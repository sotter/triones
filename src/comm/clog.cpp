/**
 * author: Triones
 * date  : 2014-08-20
 */

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>

#ifndef WIN32
#  include <unistd.h>
#  include <dirent.h>
#  include <fcntl.h>
#  include <strings.h>
#  include <errno.h>
#  include <pthread.h>
   extern int errno;
#  include <sys/syscall.h>
#else
#  include <process.h>
#  include <windows.h>
#endif

#include "monitor.h"
#include "thread.h"
#include "clog.h"


namespace triones
{

using namespace std;

#define BUF_LEN       10240
#define TIME_BUF_LEN  20
#define KEY_WORD_LEN  10
#define IP_LEN        18
#define PORT_LEN      8
#define USER_ID_LEN   20

#ifndef S_IRWXUGO
#  define S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

const char const_char[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

static string ustodecstr_log(unsigned short us)
{
	string dest;
	char buf[16] = { 0 };
	sprintf(buf, "%u", us);
	dest += buf;
	return dest;
}

/**
 * 创建多级目录, 在父目录不存在一同创建
 */
static bool mkdirs_log(char *szDirPath)
{
	struct stat stats;
	if (lstat(szDirPath, &stats) == 0 && S_ISDIR(stats.st_mode))
		return true;

	mode_t umask_value = umask(0);
	umask(umask_value);
	mode_t mode = (S_IRWXUGO & (~umask_value)) | S_IWUSR | S_IXUSR;

	char *slash = szDirPath;
	while (*slash == '/')
		slash++;

	while (1)
	{
		slash = strchr(slash, '/');
		if (slash == NULL)
			break;

		*slash = '\0';
		int ret = mkdir(szDirPath, mode);
		*slash++ = '/';
		if (ret && errno != EEXIST)
		{
			return false;
		}

		while (*slash == '/')
			slash++;
	}
	if (mkdir(szDirPath, mode))
	{
		return false;
	}
	return true;
}

//========================== 日志线程  =============================
class CLogThread : public Runnable
{
public:
	CLogThread(CLog *p)
	:_inited(false)
	{
		_pLog = p;
		start();
	}

	virtual ~CLogThread()
	{
		stop();
	}

	// 运行线程
	void run(void *param)
	{
		(void)param; // make compiler happy

		while (_inited)
		{
			// 检测文件是否存在
			_pLog->checklogfile();
			// 使用同步锁
			Synchronized s(_monitor);
			// 每隔3秒将内存日志写入文件
			_monitor.wait(5);
		}
	}

	// 发送通知信号
	void notify(void)
	{
		_monitor.notify();
	}

private:
	void start(void)
	{
		if (!_threadmgr.init(1, this, this))
		{
			perror("start clear thread failed\n");
			return;
		}
		_inited = true;
		_threadmgr.start();
		perror("start log check thread\n");
	}

	void stop(void)
	{
		perror(" stop log check thread\n");
		if (!_inited)
			return;
		_inited = false;
		_monitor.notify_end();
		_threadmgr.stop();
	}
private:
	CLog *_pLog;
	// 线程管理对象
	ThreadManager _threadmgr;
	// 是否初始化
	bool _inited;
	// 信号来阻塞
	Monitor _monitor;
};

//////////////////////////////////// 日志的类的处理  ///////////////////////////////////////////////
CLog::CLog()
{
	_stop = false;
	_log_size = 2000 * 2000;
	_log_num = 20;
	_log_level = 3;
	_file_fd = -1;
	_check_time = time(NULL);
	_log_block.offset = 0;
	_log_block.size = 0;
	// 初始化文件检测线程
	_pthread = new CLogThread(this);
}

CLog::~CLog(void)
{
	stop();
}

//如果保证其它的线程都没有在里面调用；
void CLog::stop()
{
	if (!_stop)
	{
		// 停止文件检测线程
		if (_pthread != NULL)
		{
			delete _pthread;
			_pthread = NULL;
		}
		writedisk();
		closefile();
	}
	_stop = true;
}

//安全性和效率的统一
bool CLog::print_net_msg(unsigned short log_level, const char *file, int line,
		const char *function, const char *key_word, const char * ip, int port,
		const char *user_id, const char *format, ...)
{
	if(!_stop)
		return true;
	//日志级别，1-7，数字越小日志日志级别越高。
	if (_log_level == 0)
		return false;
	if (log_level > _log_level)
		return false;

	char msg[BUF_LEN] =
	{ 0 };
	int offset = 0;
	int n = 0;

	time_t t;
	time(&t);
	struct tm local_tm;
	struct tm *tm = localtime_r(&t, &local_tm);

	snprintf(msg, TIME_BUF_LEN - 1, "%04d%02d%02d-%02d:%02d:%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec);

	offset += 17;

	n = TIME_BUF_LEN;
	while (n >= offset)
		msg[n--] = '-';
	offset = TIME_BUF_LEN;

	//key_word不会有异常内存存进来
	if (key_word == NULL)
	{
		strncpy(msg + offset, "INFO", 4);
		offset += 4;
	}
	else
	{
		strncpy(msg + offset, key_word, strlen(key_word));
		offset += strlen(key_word);
	}

	n = TIME_BUF_LEN + KEY_WORD_LEN;
	while (n >= offset)
		msg[n--] = '-';
	offset = TIME_BUF_LEN + KEY_WORD_LEN;

	if (ip != NULL)
	{
		strncpy(msg + offset, ip, IP_LEN - 1);
		offset += strlen((msg + offset));
	}
	else
	{
		strncpy(msg + offset, "null", 4);
		offset += 4;
	}
	if (port != 0)
	{
		snprintf(msg + offset, PORT_LEN - 1, ":%d", port);
		offset += strlen(msg + offset);
	}

	n = TIME_BUF_LEN + KEY_WORD_LEN + IP_LEN + PORT_LEN;
	while (n >= offset)
		msg[n--] = '-';
	offset = TIME_BUF_LEN + KEY_WORD_LEN + IP_LEN + PORT_LEN;

	if (user_id == NULL)
	{
		strncpy(msg + offset, "null", 4);
		offset += 4;
	}
	else
	{
		strncpy(msg + offset, user_id, USER_ID_LEN - 1);
		offset += strlen(msg + offset);
	}

	n = TIME_BUF_LEN + KEY_WORD_LEN + IP_LEN + PORT_LEN + USER_ID_LEN;
	while (n >= offset)
		msg[n--] = '-';
	offset = TIME_BUF_LEN + KEY_WORD_LEN + IP_LEN + PORT_LEN + USER_ID_LEN;
	va_list ap;
	if (format != NULL)
	{
		va_start(ap, format);
		vsnprintf(msg + offset, BUF_LEN - offset - 1, format, ap);
		va_end(ap);
	}

	private_log(msg, file, line, function,
			(strncmp(key_word, "RUNNING", 7) == 0));

	return true;
}

// 输出十六制的日志数据
void CLog::print_net_hex(unsigned short log_level, const char *file, int line,
		const char *function, const char * ip, int port, const char *user_id,
		const char *data, const int len)
{
	if(!_stop)
		return ;

	// 如果关闭调试日志
	if (log_level > _log_level)
		return;
	// 先确定一下是否超出长度
	if (3 * len + 256 > MAX_LOG_LENGTH || len <= 0 || data == NULL)
	{
		return;
	}

	// 使用堆上面空间效率会高一点
	char buf[MAX_LOG_LENGTH] =
	{ 0 };
	char *ptr = buf;
	char temp[1024] =
	{ 0 };

	time_t t;
	time(&t);
	struct tm local_tm;
	struct tm *tm = localtime_r(&t, &local_tm);
	// 记录来源时间
	snprintf(temp, TIME_BUF_LEN - 1, "%04d%02d%02d-%02d:%02d:%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec);
	// 添加时间
	memset(temp + 17, '-', 3);
	// 记录来源IP
	snprintf(temp + TIME_BUF_LEN, sizeof(temp) - 1 - TIME_BUF_LEN, "%s:%d--%s:",
			(ip == NULL) ? "null" : ip, port, (user_id) ? user_id : "null");
	snprintf(ptr, sizeof(buf) - 1, "%s", temp);
	ptr += strlen(temp);

	for (int i = 0; i < len; i++)
	{
		*ptr = const_char[((unsigned char) data[i] >> 4)];
		ptr++;
		*ptr = const_char[((unsigned char) data[i] & 0x0F)];
		ptr++;
	}

	// 写入十六进制数据
	private_log(buf, file, line, function, false);
}

static int getfilesize(const char *filename)
{
	struct stat buf;
	if (lstat(filename, &buf) != 0)
	{
		// 如果文件因为无法访问，返回剩余空间为0，意思是再建空间
		return 0;
	}
	return buf.st_size;
}

// 写入文件
void CLog::private_log(const char *msg, const char *file, int line,
		const char *function, bool run)
{
	if (msg == NULL)
	{
		return;
	}

	char buf[256] =
	{ 0 };
	if (file != NULL && line > 0 && function != NULL && !run)
	{
		sprintf(buf, ",%s,%s:%d\n", function, file, line);
	}
	else
	{
		sprintf(buf, "\n");
	}

	int nsize = strlen(msg);
	int nbuff = strlen(buf);

	// 是否为运行日志
	if (run)
	{
		// 如果为运行日志则直接打开文件直接写入数据
		int fd = open(_run_name.c_str(), O_CREAT | O_APPEND | O_RDWR, 0755);
		if (fd != -1)
		{
			write(fd, msg, nsize);
			write(fd, buf, nbuff);
			close(fd);
		}
	}

	_mutex.lock();

	// 检测写入的数据是否大于最大内存缓存
	if (_log_block.offset + nsize + nbuff > DEFAULT_MAXLOGSIZE)
	{
		dumpfile();
	}

	// 将数据写入内存缓存中
	memcpy(_log_block.data + _log_block.offset, msg, nsize);
	_log_block.offset = _log_block.offset + nsize;
	memcpy(_log_block.data + _log_block.offset, buf, nbuff);

	_log_block.offset = _log_block.offset + nbuff;
	_log_block.size = _log_block.size + nsize + nbuff;

	_mutex.unlock();
}

bool CLog::update_file()
{
	time_t t;
	time(&t);
	struct tm local_tm;
	struct tm *tm = localtime_r(&t, &local_tm);

	char buf[128] = { 0 };
	sprintf(buf, "%04d%02d%02d-%02d%02d%02d", tm->tm_year + 1900,
			tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	// 对目录按照日期目录进行归类处理
	char szdir[256] = { 0 };
	sprintf(szdir, "%04d/%02d/%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

	string sfile = _file_name + ".";
	sfile += buf;
	size_t npos = _file_name.rfind('/');
	if (npos != string::npos)
	{
		string sdir = _file_name.substr(0, npos + 1) + szdir;
		if (access(sdir.c_str(), 0) != 0)
		{
			mkdirs_log((char*) sdir.c_str());
		}
		sfile = sdir + _file_name.substr(npos) + ".";
		sfile += buf;
	}

	string snew = sfile;
	// 处理日志，将日志全部保留
	int pos = 0;
	while (access(snew.c_str(), 0) == F_OK)
	{
		snew = sfile + "-" + ustodecstr_log(++pos);
	}
	rename(_file_name.c_str(), snew.c_str());
	// 重新打开一次文件操作
	openfile();
	return true;
}

// 打文件操作
void CLog::openfile(void)
{
	if (_file_fd > 0)
	{
		close(_file_fd);
	}
	// 打开文件FD处理日志
	_file_fd = open(_file_name.c_str(), O_CREAT | O_APPEND | O_RDWR, 0755);
}

// 关闭文件FD
void CLog::closefile(void)
{
	if (_file_fd > 0)
	{
		close(_file_fd);
	}
	_file_fd = -1;
}

void CLog::set_log_file(const char *s)
{
	if(!_stop)
		return;

	if (s == NULL)
		return;
	_file_name = s;
	size_t pos = _file_name.rfind('/');
	if (pos != string::npos)
	{
		// 创建目录
		mkdirs_log((char*) _file_name.substr(0, pos).c_str());
	}
	_run_name = _file_name + ".running";

	openfile();
}

/////////////////////////////////////////// CDirectoryFile //////////////////////////////////////////////
// 目录文件管理对象
class CDirectoryFile
{
public:
	CDirectoryFile()
	{
	}
	virtual ~CDirectoryFile()
	{
	}

	// 检测目录数据是否正常
	bool Check(const char *root, const char *name, int log_num)
	{
		map<string, string> filemap;
		// 取得文件个数据处理
		int count = GetLogFileList(root, name, filemap);
		if (count <= log_num)
			return false;

		int num = count - log_num;
		// 遍历删除需要删除的文件
		map<string, string>::iterator it;
		for (it = filemap.begin(); it != filemap.end(); ++it)
		{
			unlink((it->second).c_str());
			if (--num <= 0)
				break;
		}
		//filemap.clear() ;

		return true;
	}

private:
	// 是否为目录
	bool isDirectory(const char *szDirPath)
	{
		struct stat stats;
		if (lstat(szDirPath, &stats) == 0 && S_ISDIR(stats.st_mode))
			return true;
		return false;
	}

	// 查找所有文件列表
	int GetLogFileList(const char* root_dir, const char *name,
			map<string, string> &filemap)
	{
		DIR* dir_handle = opendir(root_dir);
		if (dir_handle == NULL)
			return 0;

		int count = 0;
		char buf[1024] = { 0 };
		struct dirent* entry = (struct dirent*) buf;
		struct dirent* dir = NULL;

		while (dir_handle)
		{
			int ret_code = readdir_r(dir_handle, entry, &dir);
			if (ret_code != 0 || dir == NULL)
			{
				break;
			}

			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			{
				continue;
			}

			char szbuf[1024] = { 0 };
			sprintf(szbuf, "%s/%s", root_dir, dir->d_name);
			// 如果是目录则直接递归遍历
			if (isDirectory(szbuf))
			{
				count += GetLogFileList(szbuf, name, filemap);
				continue;
			}

			// 如果是本日志文件不添加处理
			if (strcmp(dir->d_name, name) == 0)
			{
				continue;
			}

			// 如果为历史日志文件添加处理队列中
			if (strncmp(dir->d_name, name, strlen(name)) != 0)
			{
				continue;
			}
			// printf( "add file: %s\n", szbuf ) ;
			filemap.insert(make_pair(dir->d_name, szbuf));

			++count;
		}
		closedir(dir_handle);

		return count;
	}
};

// 将内存数据写入磁盘
void CLog::writedisk(void)
{
	_mutex.lock();
	dumpfile();
	_mutex.unlock();
}

// 检测日志数据
void CLog::checklogfile(void)
{
	writedisk();
	checkfile();
}

// 将内存日志写入文件
void CLog::dumpfile(void)
{
	// 这里使用日志双缓存区操作，这样只需的切换一下缓存区就可以了
	if (_log_block.offset == 0)
	{
		return;
	}

	// 打开文件，如果该文件不能被打开，就返回错误
	if (_file_fd <= 0)
		openfile();
	if (_file_fd > 0)
	{
		int size = 0, cnt = 0;
		// 如果写入失败重试两次处理
		while (size <= 0 && ++cnt < 2)
		{
			// 写入消息
			size += write(_file_fd, _log_block.data, _log_block.offset);
			if (size <= 0)
				openfile();
			// 如果无法打开文件描述符就直接退出了
			if (_file_fd <= 0)
				break;
		}
	}
	else
	{
		printf("%s\n", _log_block.data);
	}
	_log_block.offset = 0;

	// 如果记录大于最大值
	if (_log_block.size > DEFAULT_MAXLOGSIZE)
	{
		// 取得文件大小
		if (getfilesize(_file_name.c_str()) > _log_size)
		{
			// 重命名文件
			update_file();
		}
		_log_block.size = 0;
	}
}

// 检测日志个数是否超出限制
void CLog::checkfile(void)
{
	// printf( "check log file\n" ) ;
	// 如果需要保留的文件数有限制则只保留这么长时间的文件数
	if (_log_num <= 0)
		return;

	time_t now = time(NULL);
	// 如果没有到时间重新处理,每隔5分钟检测一次
	if (now - _check_time < 300)
	{
		return;
	}
	_check_time = now;

	size_t pos = _file_name.rfind('/');
	if (pos == string::npos)
	{
		return;
	}

	string path = _file_name.substr(0, pos);
	string name = _file_name.substr(pos + 1);

	// 目录文件管理对象
	CDirectoryFile dirfile;
	dirfile.Check(path.c_str(), name.c_str(), _log_num);
}

void debug_printf(const char *file, int line, const char *fmt, ...)
{
	va_list ap;

#ifdef  WIN32
	DWORD pid = GetCurrentThreadId();
#else
	//pthread_t pid = pthread_self();
#ifdef _UNIX
	pid_t pid = getpid();
#else
	pid_t pid = (long) syscall(__NR_gettid);
#endif
#endif

	fprintf(stdout, "(%s:%d:PID %d:TID %d)\n", file, line, getpid(), pid);
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	fprintf(stdout, "\n");
	fflush(stdout);
}

void info_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);

	fprintf(stdout, "\n");
	fflush(stdout);
}

} // namespace triones

