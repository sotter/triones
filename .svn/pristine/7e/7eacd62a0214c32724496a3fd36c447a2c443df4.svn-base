/******************************************************
 *   FileName: vos_event.cpp
 *     Author: Triones  2012-7-6 
 *Description:
 *******************************************************/
#include "vos_event.h"
#include <stdio.h>
#include <string.h>
#ifdef __LINUX
#include <errno.h>
#endif
namespace vos
{
Event::Event(bool ismanual_reset/* = false*/, bool initial_state /*= false*/,
		const char* name /*= NULL*/)
{
	init_event(ismanual_reset, initial_state, name);
}

Event::~Event()
{
	destroy_event();
}

void Event::init_event(bool ismanual_reset/* = false*/, bool initial_state/* = false*/,
		const char* name /*= NULL*/)
{

	UNUSED(ismanual_reset);
	UNUSED(initial_state);
    UNUSED(name);

#ifdef __WINDOWS
	if ( name != NULL )
	{
		char szMaxName[MAX_PATH] = { 0 };
		snprintf(szMaxName, MAX_PATH - 1, "event%s", name);
		szMaxName[MAX_PATH - 1] = 0;
		hEvent=CreateEvent(NULL,ismanual_reset,initial_state,szMaxName);
	}
	else
	{
		hEvent=CreateEvent(NULL,ismanual_reset,initial_state,name);
	}

	if ( hEvent == NULL )
	{
		DWORD dwError = GetLastError();
	}
#else
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, (pthread_condattr_t *) PTHREAD_PROCESS_PRIVATE);
#endif
	_wait_event = false;
	_set_event_count = 0;
}

void Event::destroy_event()
{
#ifdef __WINDOWS
	CloseHandle(hEvent);
#else
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
#endif
	_wait_event = false;
	_set_event_count = 0;
}

void Event::set_event()
{
#ifdef __WINDOWS
	::SetEvent(hEvent);
	_set_event_count++;
#else
	pthread_mutex_lock(&m_mutex);
	_set_event_count++;
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
#endif
}

void Event::pulse_event()
{
#ifdef __WINDOWS
	::PulseEvent(hEvent);
	_set_event_count++;
#else
	pthread_mutex_lock(&m_mutex);
	_set_event_count++;
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
#endif
}

int Event::wait_event(int time_out)
{
	int iRes = 0;

	_wait_event = true;

#ifdef __WINDOWS
	unsigned int m_Res;
	m_Res=WaitForSingleObject(hEvent, time_out);

	if(m_Res==WAIT_OBJECT_0)
	iRes= EVENT_HAVEEVNET;

	if ( m_Res == WAIT_TIMEOUT)
	iRes = EVNET_TIMEOUT;

	if(m_Res==WAIT_ABANDONED)
	iRes=EVENT_ERROR;

	_set_event_count--;

	//if(_set_event_count<0)_set_event_count=0;
#else

	pthread_mutex_lock(&m_mutex);

	if (_set_event_count == 0)
	{
		struct timespec m_timv;

		m_timv.tv_sec = time(NULL) + time_out / 1000;
		m_timv.tv_nsec = (time_out % 1000) * 1000000;

		iRes = pthread_cond_timedwait(&m_cond, &m_mutex, &m_timv);

		if (iRes == 0)
		{
			iRes = EVENT_HAVEEVNET;
			if (_set_event_count > 0)
				_set_event_count--;
		}
		else
		{
            //һ��Ҫ��errno.h�����, ��Ȼ����ΪETIMEDOUT ��pthread.h�еĶ��塣
			iRes = (iRes == ETIMEDOUT) ?  EVNET_TIMEOUT : EVENT_ERROR;
		}
	}
	else
	{
		if (_set_event_count > 0)
			_set_event_count--;
		iRes = EVENT_HAVEEVNET;
	}

	pthread_mutex_unlock(&m_mutex);

#endif

	_wait_event = false;
	return iRes;
}

int Event::wait_event_always()
{
	int iRes = 0;

	_wait_event = true;

#ifdef __WINDOWS
	unsigned int m_Res;
	m_Res=WaitForSingleObject(hEvent,INFINITE);
	if(m_Res==WAIT_OBJECT_0)iRes=1;
	if(m_Res==WAIT_ABANDONED)iRes=-1;
	_set_event_count--;
	if(_set_event_count<0)_set_event_count=0;
#else
	pthread_mutex_lock(&m_mutex);
	if (_set_event_count == 0)
	{
		iRes = pthread_cond_wait(&m_cond, &m_mutex);
		if (iRes == 0)
		{
			if (_set_event_count > 0)
				_set_event_count--;
			iRes = 1;
		}
		else
			iRes = -1;
	}
	else if (_set_event_count > 0)
		_set_event_count--;
	pthread_mutex_unlock(&m_mutex);
#endif
	_wait_event = false;
	return iRes;
}

bool Event::is_wait_event()
{
	return _wait_event;
}

int Event::get_set_event_count()
{
	return _set_event_count;
}

void Event::reset_event()
{
#ifdef __WINDOWS
	//	if(IsWaitEvent())
	::ResetEvent(hEvent);
#else

#endif
	_wait_event = false;
	_set_event_count = 0;
}

void Event::set_wait_event(bool wait)
{
	_wait_event = wait;
}

} /* namespace vos */
