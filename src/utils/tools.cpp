/**
 * author: Triones
 * date  : 2014-08-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef _NI_WIN_
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iconv.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <netinet/in.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "tools.h"

namespace triones
{

#define OUTLEN  255

#define MINUTE  60
#define HOUR    (60*MINUTE)
#define DAY     (24*HOUR)
#define YEAR    (365*DAY)

/* interestingly, we assume leap-years */
static int month[12] = {
		0,
		DAY*(31),
		DAY*(31+29),
		DAY*(31+29+31),
		DAY*(31+29+31+30),
		DAY*(31+29+31+30+31),
		DAY*(31+29+31+30+31+30),
		DAY*(31+29+31+30+31+30+31),
		DAY*(31+29+31+30+31+30+31+31),
		DAY*(31+29+31+30+31+30+31+31+30),
		DAY*(31+29+31+30+31+30+31+31+30+31),
		DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

long kernel_mktime(struct tm * tm)
{
	long res;
	int year;

	year = tm->tm_year - 70;

	/* magic offsets (y+1) needed to get leapyears right.*/

	res = YEAR * year + DAY * ((year + 1) / 4);
	res += month[tm->tm_mon];

	/* and (y+2) here. If it wasn't a leap-year, we have to adjust */

	if (tm->tm_mon > 1 && ((year + 2) % 4))
		res -= DAY;
	res += DAY * (tm->tm_mday - 1);
	res += HOUR * tm->tm_hour;
	res += MINUTE * tm->tm_min;
	res += tm->tm_sec;

	//减去八个小时的时间
	return res - (8 * 3600);
}

// 检测IP的合法性
bool check_addr(const char *ip)
{
	if (ip == NULL)
		return false;
	if (inet_addr(ip) == (in_addr_t) -1)
		return false;
	return true;
}

// 这里主要处理分析路径中带有 env:LBS_ROOT/lbs 之类的路径
bool get_env_path(const char *value, char *szbuf)
{
	if (value == NULL)
		return false;

	char buf[1024] =
	{ 0 };
	strcpy(buf, value);

	if (strncmp(buf, "env:", 4) == 0)
	{
		char *q = strstr(buf, "/");
		if (q == NULL)
		{
			q = strstr(buf, "\\");
		}
		if (q == NULL)
		{
			return false;
		}
		*q = 0;

		sprintf(szbuf, "%s/%s", getenv(buf + 4), q + 1);

	}
	else
	{
		sprintf(szbuf, "%s", buf);
	}
	return true;
}

/**
 *  取得当前环境对象路径,
 *	env 为环境对象名称，buf 存放路径的缓存, sz 为附加后缀, def 默认的中径
 */
const char * get_run_path(const char *env, char *buf, const char *sz,
		const char *def)
{
	if (env == NULL)
	{
		sprintf(buf, "%s", def);
		return buf;
	}

	char *ptr = getenv(env);
	if (ptr == NULL)
	{
		sprintf(buf, "%s", def);
		return buf;
	}

	if (sz != NULL)
	{
		sprintf(buf, "%s/%s", ptr, sz);
	}
	else
	{
		sprintf(buf, "%s", ptr);
	}
	return buf;
}

// 取得默认的CONF路径
const char * get_conf_path(const char *env, char *buf, const char *sz,
		const char *def, const char *conf)
{
	char temp[512] =
	{ 0 };
	get_run_path(env, temp, sz, def);
	sprintf(buf, "%s/%s", temp, conf);
	return buf;
}

// 写入文件操作
bool append_file(const char *szName, const char *szBuffer, const int nLen)
{
#ifdef _WIN32
	FILE *fp = fopen( szName, "a+" );
	if ( fp == NULL )
	{
		return false;
	}
	fwrite( szBuffer, nLen, 1, fp );
	fclose( fp );
#else
	int fp = open(szName, O_CREAT | O_APPEND | O_WRONLY);
	if (fp < 0)
	{
		return false;
	}
	write(fp, szBuffer, nLen);
	close(fp);
	chmod(szName, 0777);
#endif

	return true;
}

// 写入文件操作
bool write_file(const char *szName, const char *szBuffer, const int nLen)
{
#ifdef _WIN32
	FILE *fp = fopen( szName, "wb" );
	if ( fp == NULL )
	{
		return false;
	}
	fwrite( szBuffer, nLen, 1, fp );
	fclose( fp );
#else
	int fp = open(szName, O_CREAT | O_TRUNC | O_WRONLY);
	if (fp < 0)
	{
		return false;
	}
	write(fp, szBuffer, nLen);
	close(fp);
	chmod(szName, 0777);
#endif

	return true;
}

// 读取文件
char *read_file(const char *szFile, int &nLen)
{
	char *szBuffer = NULL;
#ifdef _WIN32

	FILE* fp = fopen( szFile , "rb" );
	if ( fp != NULL )
	{
		int len = 0;
		fseek( fp , 0 , SEEK_END );
		len = ftell( fp );

		szBuffer = new char[len+1];

		fseek( fp , 0 , SEEK_SET );
		fread( szBuffer , 1 , len , fp );
		szBuffer[len] = 0;

		nLen = len;

		fclose( fp );
	}
	else
	{
		nLen = 0;
		return NULL;
	}

#else

	int fp = open(szFile, O_RDONLY);

	if (fp >= 0)
	{
		// 先得到文件的大小
		struct stat buf;
		fstat(fp, &buf);

		int len = buf.st_size;

		szBuffer = new char[len + 1];

		read(fp, szBuffer, len);
		szBuffer[len] = 0;

		nLen = len;

		close(fp);
	}
	else
	{
		nLen = 0;
		return NULL;
	}

#endif

	return szBuffer;
}

} // namespace triones
