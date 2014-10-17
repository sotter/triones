/**
 * author: Triones
 * date  : 2014-08-20
 * desc  : 鏃ュ織绠＄悊瀵硅薄,鏃ュ織鍙互鏍规嵁绾у埆鏉ヨ缃槸鍚﹁澶勭悊锛屽鏋滅骇鍒秺楂樺垯鏁板�瓒婂皬
 *         褰撹缃骇鍒秺楂樻椂鏄剧ず璁板綍鐨勬棩蹇楀氨瓒婂皯锛屽綋涓洪浂鏃跺叧闂棩蹇�1绾ф棩蹇楀彧涓洪敊璇拰璀﹀憡锛�绾т负涓�父鐢ㄦ暟鎹祦绋嬶紝3绾т负璋冭瘯绾у埆
 */
#ifndef __TRIONES_COMLOG_H__
#define __TRIONES_COMLOG_H__
#include "clog.h"

namespace triones
{

#ifdef _XDEBUG
#define OUT_DEBUG( fmt, ... )       printf( fmt, ## __VA_ARGS__ )
#define DEBUG_PRT(fmt, ...)         debug_printf(__FILE__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define OUT_DEBUG( fmt, ... )
#define DEBUG_PRT(fmt, ...)
#endif
#define INFO_PRT(fmt, ...)          info_printf(fmt, ## __VA_ARGS__)

#define CHGLOG(pathname)      CLog::instance()->set_log_file(pathname);
#define CHGLOGSIZE(newsize)   CLog::instance()->set_log_size(newsize);
#define CHGLOGNUM(log_num)    CLog::instance()->set_log_num(log_num);
#define FFLUSH
#define SETLOG(pathname)      CLog::instance()->set_log_file(pathname);
#define SETLOGSIZE(newsize)   CLog::instance()->set_log_size(newsize);
#define SETLOGNUM(log_num)    CLog::instance()->set_log_num(log_num);
#define SETLOGLEVEL(level)    CLog::instance()->set_log_level(level);
#define LOGSTOP()             CLog::instance()->stop();
#define LOG_NUM_LEVEL(level)  level, __FILE__, __LINE__, __FUNCTION__
#define LOG_NULL_LEVEL(level) level, NULL, 0, NULL

//// 涓�骇鏃ュ織锛岄敊璇姤璀︼紝杩欑被闇�璁板綍閿欒鍑虹幇鐨勬墍鍦ㄦ枃浠惰鍙蜂互鍙婂嚱鏁板悕绉�#define OUT_WARNING(ip,port,user_id,msg,...)
//		CLog::instance()->CLog::print_net_msg(LOG_NUM_LEVEL(1),"WARN",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_ERROR(ip,port,user_id,msg,...)\
		CLog::instance()->CLog::print_net_msg(LOG_NUM_LEVEL(1),"ERROR",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_ASSERT(ip,port,user_id,msg,...)\
		CLog::instance()->CLog::print_net_msg(LOG_NUM_LEVEL(1),"ASSERT",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_INFO(ip,port,user_id,msg,...)\
		CLog::instance()->CLog::print_net_msg(LOG_NUM_LEVEL(1),"INFO",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_RUNNING(ip,port,user_id,msg,...)\
		CLog::instance()->CLog::print_net_msg(LOG_NUM_LEVEL(1),"RUNNING",ip,port,user_id,msg,  ## __VA_ARGS__)

//// 浜岀骇鏃ュ織锛屾祦绋嬬畝鍗曟暟鎹�#define OUT_INFO( ip, port, user_id, msg, ...)
//		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(2),"INFO",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_RECV( ip, port, user_id, msg, ...)\
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(2),"RECV",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_SEND( ip, port, user_id, msg, ...)\
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(2),"SEND",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_CONN( ip, port, user_id, msg, ...)\
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(2),"CONN",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_DISCONN( ip, port, user_id, msg, ...)\
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(2),"DISCON",ip,port,user_id,msg,  ## __VA_ARGS__)

//// 涓夌骇鏃ュ織涓鸿皟璇曞鐞�#define OUT_RECV3( ip, port, user_id, msg, ...)
//		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(3),"RECV",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_SEND3( ip, port, user_id, msg, ... ) \
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(3),"SEND",ip,port,user_id,msg,  ## __VA_ARGS__)
#define OUT_PRINT( ip, port, user_id,msg, ... ) \
		CLog::instance()->CLog::print_net_msg(LOG_NULL_LEVEL(3),"PRINT",ip,port,user_id,msg,  ## __VA_ARGS__)
// 杈撳嚭鍗佸叚杩涘埗鏁版嵁
#define OUT_HEX( ip, port, user_id, data, len )  \
		CLog::instance()->print_net_hex(LOG_NULL_LEVEL(3), ip, port, user_id, data, len )

}  // namespace triones

#endif // #ifndef __TRIONES_COMLOG_H__


