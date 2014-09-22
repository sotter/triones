/******************************************************
 *   FileName: vos_thread.cpp
 *     Author: Triones  2012-7-5 
 *Description:
 *******************************************************/

#include "vos_thread.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
namespace vos
{
/* static */
THREAD_T make_thread(void *fun, void *arg)
{
	THREAD_T tid;
#ifdef	__WINDOWS
	unsigned thread_id;
	tid = (THREAD_T)_beginthreadex(0, 0, (unsigned int (__stdcall *)(void *) )fun, arg, 0, &thread_id);
	return tid;
#else
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	if (0 == pthread_create(&tid, 0, (void*(*)(void*))fun, arg))
	{
		return tid;
	}
	return -1;
#endif

}

/* static */
int wait_thread(THREAD_T tid)
{
#ifdef	__WINDOWS
	return WAIT_OBJECT_0 == WaitForSingleObject(tid, INFINITE);
#else
	void** value_ptr = NULL;
	return 0 == pthread_join(tid, value_ptr);
#endif
}

/* static */
int stop_thread(THREAD_T tid)
{
	//待添加代码
    tid = 0;
    return 0;
}

#ifdef __WINDOWS
unsigned int  WINAPI Thread::the_thread(void *param)
{
#else
void*  Thread::the_thread(void *param)
{
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
#endif
    Thread *thread = (Thread*)param;
    thread->_state = running;
    thread->run();
    thread->_state = stopped;
    thread->_stop_event.set_event();
#ifdef __WINDOWS
	return 0;
#else
	return NULL;
#endif
}

int Thread::start()
{
    _tid = make_thread((void*)(the_thread), (void*)this);
	return 0;
}

int Thread::stop(int timeout)
{
	set_exit();
    int ret;
	if(EVNET_TIMEOUT == (ret =  _stop_event.wait_event(timeout)))
	{
		this->exit();
	}
	printf("call this->exit() \n");
    return 0;
}

int Thread::exit()
{
#ifndef __WINDOWS
	pthread_cancel(_tid);
#else
//	_endthreadex(_tid);
#endif
	_tid = 0;
    _exit = true;
    _state = exited;
	return 0;
}

int ThreadPool::regist(int type_index, RunThread *run, int num)
{
	for(int i = 0; i < num; i++)
	{
        Thread *thread = new Thread;
        thread->_type = type_index;
        thread->_run_thread = run;
        _vec_thread.push_back(thread);
	}
	return 0;
}

int ThreadPool::start()
{
    for(int i = 0; i < (int)_vec_thread.size(); i++)
    {
    	_vec_thread[i]->start();
    }

    return 0;
}
/*************
 * 在发送结束信号timeout的时间内,线程还没有结束，变强行杀死.
 **********************/
int ThreadPool::stop(int timeout)
{
    for(int i = 0; i < (int)_vec_thread.size(); i++)
    {
    	_vec_thread[i]->stop(timeout);
    }
    return 0;
}

}

