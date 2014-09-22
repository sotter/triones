/******************************************************
 *   FileName: vos_thread.h
 *     Author: Triones  2012-7-5 
 *Description:
 *Description: (1)给类提供线程服务
 *Description: (2)封装windows的线程函数，提供方法。
 *******************************************************/

#ifndef VOS_THREAD_H_
#define VOS_THREAD_H_

#ifdef __WINDOWS
#include <process.h>
typedef void* THREAD_T;
#else

#include <sys/time.h>
#include <time.h>
#include <pthread.h>
typedef pthread_t THREAD_T;
#endif

#include <vector>
#include "vos_event.h"
namespace vos
{
#define  MAX_THREAD 128

/********************************
 *  屏蔽Linux 和 Winodows区别的方法函数
 *******************************/
extern THREAD_T make_thread(void *fun, void *arg);
extern int wait_thread(THREAD_T tid);
//static int exit_thread(THREAD_T tid);

class ThreadManage;
class Thread;
class RunThread
{
public:
    virtual ~RunThread(){};
    virtual int run_thread(int type) = 0;
};

class Thread
{
public:
	enum State{init, running, exited, stopped};

    Thread(): _id(0), _tid(0), _state(init), _exit(false){}
    ~Thread(){};

	int start();

	/*********************************
	 * 在规定的时间内，线程如果没有执行完，强行结束线程。
	 * ******************************/
    int stop(int timeout);

    //提供给应用层的使用者用的
    int isexit(){return _exit;}

private:
	void run()
	{
		_run_thread->run_thread(_type);
	}
    int exit();
    void set_exit(){_exit = true;}
#ifdef __WINDOWS
	static unsigned int WINAPI the_thread(void *param);
	
#else
	static void* the_thread(void *param);
#endif

public:
    vos::Event _stop_event;
    int _type;
    int _id;            //序列标识号
	THREAD_T _tid;
	State _state;
	RunThread *_run_thread;
	bool _exit;
};

class ThreadPool
{
#define max_thread_num 64
public:
    /***
     * 注册线程，type_index标识线程的类型， 此种类型的线程注册多少个。
     */
	int regist(int type_index, RunThread *run,  int num = 1);
    int start();
    /*************
     * 在发送结束信号timeout的时间内,线程还没有结束，变强行杀死.
     **********************/
    int stop(int timeout = 0);
private:

	std::vector<Thread*> _vec_thread;
};

} /* namespace vos */
#endif /* VOS_THREAD_H_ */
