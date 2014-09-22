/**
 * author: Triones
 * date  : 2014-08-20
 */

#ifndef __TRIONES_CLOG_H__
#define __TRIONES_CLOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  if defined(_MSC_VER)
#     pragma warning(disable:4786)    // identifier was truncated in debug info
#     pragma warning(disable:4996)    // identifier was truncated in debug info
#  endif
#else
#  include <pthread.h>
#endif

#include "singleton.h"
#include "../thread/mutex.h"

using namespace triones;

namespace triones
{

#define LOG_FILE_NUM        20
#define MAX_LOG_LENGTH      10240
#define MAX_BACK_LOG        2
#define DEFAULT_MAXLOGSIZE  1024*1024

void debug_printf(const char *file, int line, const char *fmt, ...);
void info_printf(const char *fmt, ...);

class CLogThread;
class CLog: public Singleton<CLog>
{
	struct LogBlock
	{
		unsigned int size;
		unsigned int offset;
		char data[ DEFAULT_MAXLOGSIZE + 1];
	};

public:
	CLog();
	virtual ~CLog();

	// 输出日志信息
	bool print_net_msg(unsigned short log_level, const char *file, int line,
			const char *function, const char *key_word, const char * ip,
			int port, const char *user_id, const char *format, ...);
	// 输出十六制的日志数据
	void print_net_hex(unsigned short log_level, const char *file, int line,
			const char *function, const char * ip, int port,
			const char *user_id, const char *data, const int len);

	void set_log_file(const char *s);
	void set_log_num(unsigned short num = 20)
	{
		_log_num = num;
	}
	void set_log_level(int loglevel = 3)
	{
		_log_level = loglevel;
	}
	void set_log_size(int newsize = 20)
	{
		if (newsize < 1000)
			_log_size = newsize * 1000 * 1000;
		else
			_log_size = 20 * 1000 * 1000;
	}
	// 检测日志文件个数
	void checklogfile(void);

private:
	bool update_file();
	// 写入文件
	void private_log(const char *msg, const char *file, int line,
			const char *function, bool run);
	// 打文件操作
	void openfile(void);
	// 关闭文件FD
	void closefile(void);
	// 将内存日志写入文件
	void dumpfile(void);
	// 检测日志个数是否超出限制
	void checkfile(void);
	// 将内存数据写入磁盘
	void writedisk(void);

private:
	std::string _file_name;   // 记录日志的的文件名
	long  _log_size;          // 日志文件大小
	int   _log_num;           // 最多记录日志文件个数
	int   _log_level;         // 日志级别
	int   _file_fd;           // 打开文件的文件ID
	int   _check_time;        // 最后一次检测时间
	Mutex _mutex;             // 操作日志锁
	CLogThread *_pthread;     // 定时处理线程对象
	LogBlock    _log_block;   // 当前日志记录数据队列
	std::string _run_name;    // 运行日志统计
};

} // namespace triones

#endif // #ifndef __TRIONES_CLOG_H__

