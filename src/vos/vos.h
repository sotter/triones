/******************************************************
*   FileName: vos.h
*     Author: Triones  2012-7-2 
*Description:
*******************************************************/

#ifndef VOS_H_
#define VOS_H_

#if defined(__WINDOWS)
#define EINPROGRESS	WSAEINPROGRESS
#define EWOULDBLOCK	WSAEWOULDBLOCK
#define ETIMEDOUT	    WSAETIMEDOUT

#define snprintf	    _snprintf
//# define vsnprintf    _vsnprintf
//# define strcasecmp	  _stricmp
//# define strncasecmp	_strnicmp


typedef int  socklen_t;

#endif

typedef signed char int8;
typedef unsigned char   uint8;
typedef short  int16;
typedef unsigned short  uint16;
typedef int  int32;
typedef unsigned   uint32;
typedef long long  int64;
typedef unsigned long long   uint64;


/* INT 类型表示错误码，理论上来讲错误已经足够了。 */
#define OK          			    0
#define ERROR                       -1

/*以256为单位分割错误码的使用， 0x00 00 00 00*/
#define ERROR_BASE       		     ERROR
#define ERROR_PARAM     		    (ERROR_BASE - 1)
#define ERROR_MALLOC    		    (ERROR_BASE - 2)
#define BUFFER_NO_SPACE             (ERROR_BASE - 3)   //APP读写缓冲区满了，

/*10-50 给socket用*/
#define ERROR_SOCK_BASE              -0x10000
#define ERROR_SOCK_INIT			    (ERROR_SOCK_BASE - 10)
#define ERROR_SOCK_BIND			    (ERROR_SOCK_BASE - 11)
#define ERROR_SOCK_LISTEN		    (ERROR_SOCK_BASE - 12)
#define ERROR_SOCK_ACCEPT           (ERROR_SOCK_BASE - 13)
#define ERROR_SOCK_EAGAIN		    (ERROR_SOCK_BASE - 14)
#define ERROR_SOCK_EPIPE		    (ERROR_SOCK_BASE - 15)
#define ERROR_SOCK_CLOSE		    (ERROR_SOCK_BASE - 16)
#define ERROR_SOCK_EPOLL            (ERROR_SOCK_BASE - 17)
#define ERROR_SOCK_SELECT           (ERROR_SOCK_EPOLL)
#define ERROR_SOCK_SET_OPT          (ERROR_SOCK_BASE - 18)
#define ERROR_SOCK_CONN             (ERROR_SOCK_BASE - 19)
#define ERROR_SOCK_CONN_TIMEOUT     (ERROR_SOCK_BASE - 20)

#define ERROR_SOCK_REMOTE_CLOSE     (ERROR_SOCK_BASE - 21)

#define ERROR_SOCK_DIS_CONN         (ERROR_SOCK_BASE - 22)
#define ERROR_SOCK_EGAIN            (ERROR_SOCK_BASE - 23)
#define ERROR_SOCK_RD_DEFAULT       (ERROR_SOCK_BASE - 25)
#define ERROR_SOCK_WR_DEFAULT       (ERROR_SOCK_BASE - 26)
#define ERROR_SOCK_CONN_ERROR       (ERROR_SOCK_BASE - 27)
#define ERROR_SOCK_READ_TIMEOUT     (ERROR_SOCK_BASE - 28)
#define ERROR_SOCK_WRITE_TIMEOUT    (ERROR_SOCK_BASE - 29)

#define ERROR_EPOLL_CREATE          (ERROR_SOCK_BASE - 30)
#define ERROR_EPOLL_ADD             (ERROR_SOCK_BASE - 31)
#define ERROR_EPOLL_DEL             (ERROR_SOCK_BASE - 32)
#define ERROR_EPOLL_WAIT            (ERROR_SOCK_BASE - 33)
#define ERROR_EPOLL_MOD             (ERROR_SOCK_BASE - 34)

#define ERROR_EVENT_BASE              -0x20000

#define EVNET_TIMEOUT                (ERROR_EVENT_BASE -2)
#define EVENT_HAVEEVNET              (ERROR_EVENT_BASE -3)
#define EVENT_ERROR                  (ERROR_EVENT_BASE -4)

#define NI_ERR_SEM_OK						0
#define NI_ERR_SEM_SYS						-1
#define NI_ERR_SEM_NOTINIT					-2
#define NI_ERR_SEM_ALREADYINIT				-3

#define NI_ERR_THREAD_CANTINITMUTEX			-1
#define NI_ERR_THREAD_CANTSTARTTHREAD		-2
#define NI_ERR_THREAD_THREADFUNCNOTSET		-3
#define NI_ERR_THREAD_NOTRUNNING			-4
#define NI_ERR_THREAD_ALREADYRUNNING		-5

//比一般高2个等级
#define NI_THREAD_PRIORITY_HIGHEST			THREAD_PRIORITY_HIGHEST
//比一般优先级高一个等级
#define NI_THREAD_PRIORITY_ABOVE_NORMAL		THREAD_PRIORITY_ABOVE_NORMAL
//一般等级
#define NI_THREAD_PRIORITY_NORMAL			THREAD_PRIORITY_NORMAL
//比一般低一个等级
#define NI_THREAD_PRIORITY_BELOW_NORMA		THREAD_PRIORITY_BELOW_NORMAL
//比一般低2个等级
#define NI_THREAD_PRIORITY_LOWEST			THREAD_PRIORITY_LOWEST


#define IO_READ_EVENT    		1
#define IO_WRITE_EVENT   		2
#define IO_EXCEPT_EVENT  		4

#define EPOLL_MAX_NUM    		128
#define EPOLL_EVENT_NUM  		128 	/*epoll 能同时触发的事件*/

#define LISTEN_BACK_LOG         16      /*listen的连接队列*/

#define __MAKE_CORE		              \
    struct rlimit sLimit;             \
	sLimit.rlim_cur = -1;             \
	sLimit.rlim_max = -1;             \
	setrlimit(RLIMIT_CORE, &sLimit);

#define UNUSED(expr) do { (void)(expr); } while (0)


#endif /* VOS_H_ */
