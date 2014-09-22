/******************************************************
 *   FileName: vos_event.h
 *     Author: Triones  2012-7-6 
 *Description:
 *******************************************************/

#ifndef VOS_EVENT_H_
#define VOS_EVENT_H_

#include "vos.h"

#ifdef __LINUX
#include <pthread.h>
#include <semaphore.h>

#else
#include <windows.h>
#endif

namespace vos
{

class Event
{
private:
	bool _wait_event;
	int _set_event_count;
private:
#ifdef __WINDOWS
    HANDLE hEvent;
#else
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
#endif
private:
    void init_event(bool ismanual_reset = false, bool initial_state = false, const char* name = NULL);
	void destroy_event();
public:
	Event(bool ismanual_reset = false, bool initial_state = false, const char* name = NULL);
	virtual ~Event();
public:
	void set_event();
	void pulse_event();
	//µ•Œª «∫¡√Î
	int  wait_event(int dwTimeOut);
	int  wait_event_always();
	bool is_wait_event();
	int  get_set_event_count();
	void reset_event();
	void set_wait_event(bool bWait=true);
};

} /* namespace vos */
#endif /* VOS_EVENT_H_ */
