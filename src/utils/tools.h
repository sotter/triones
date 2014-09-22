/**
 * author: Triones
 * date  : 2014-08-28
 */
#ifndef __TRIONES_TOOLS_H__
#define __TRIONES_TOOLS_H__

#include <time.h>

namespace triones
{

// 将TM对象转换成8时间的时间
extern long kernel_mktime(struct tm * tm) ;


// 检测IP的有效性
bool check_addr( const char *ip ) ;

// 这里主要处理分析路径中带有 env:LBS_ROOT/lbs 之类的路径
bool get_env_path( const char *value, char *szbuf ) ;

/**
 *  取得当前环境对象路径,
 *	env 为环境对象名称，buf 存放路径的缓存, sz 为附加后缀, def 默认的中径
 */
const char * get_run_path( const char *env, char *buf, const char *sz, const char *def ) ;

// 取得默认的CONF路径
const char * get_conf_path( const char *env, char *buf, const char *sz, const char *def, const char *conf ) ;

// 追加写入文件操作
bool append_file( const char *szName, const char *szBuffer, const int nLen ) ;

// 创建新文件写入
bool write_file( const char *szName, const char *szBuffer, const int nLen ) ;

// 读取文件
char *read_file( const char *szFile , int &nLen ) ;


} // namespace triones

#endif // #ifndef __TRIONES_TOOLS_H__

