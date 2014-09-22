/******************************************************
 *   FileName: vos_lock.h
 *     Author: Triones  2012-7-5
 *Description:
 *******************************************************/

#ifndef  __VOS_LOCKT_H__
#define  __VOS_LOCKT_H__

#ifdef __WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

#include <errno.h>

namespace vos
{

class Mutex
{
#if defined(__WINDOWS)
public:
	Mutex()
	{
		::InitializeCriticalSection(&m_cs);
	}
	virtual ~Mutex()
	{
		::DeleteCriticalSection(&m_cs);
	}
	virtual bool lock()
	{
		::EnterCriticalSection(&m_cs);
		return true;
	}
	virtual bool unlock()
	{
		::LeaveCriticalSection(&m_cs);
		return true;
	}
private:
	mutable CRITICAL_SECTION m_cs;
#else

public:
	Mutex()
	{
		pthread_mutex_init(&_mutex, NULL);
	}
	virtual ~Mutex()
	{
		pthread_mutex_destroy(&_mutex);
	}
	virtual bool lock()
	{
		return 0 == pthread_mutex_lock(&_mutex);
	}
	virtual bool trylock()
	{
		return 0 == pthread_mutex_trylock(&_mutex);
	}
	virtual bool unlock()
	{
		return 0 == pthread_mutex_unlock(&_mutex);
	}
private:
	pthread_mutex_t _mutex;
#endif
};

class CRWLock
{
#if defined(__WINDOWS)
public:
	DWORD m_rdcnt; // number of active readers
	DWORD m_rdwaitcnt;// number of waiting readers
	DWORD m_wrcnt;// number of active writers
	DWORD m_wrwaitcnt;// number of waiting writers
	HANDLE rdEv, wrEv;// reader and writer events
	CRITICAL_SECTION rwcs;// R/W lock critical section
public:
	CRWLock()
	{
		m_rdcnt = m_rdwaitcnt = m_wrcnt = m_wrwaitcnt = 0;
		InitializeCriticalSection(&rwcs);
		rdEv = CreateEvent(NULL, FALSE, TRUE, NULL);
		wrEv = CreateEvent(NULL, FALSE, TRUE, NULL);
	}

	~CRWLock()
	{
		CloseHandle(rdEv);
		CloseHandle(wrEv);
		DeleteCriticalSection(&rwcs);
	}

	bool lock_read(DWORD timeoms=INFINITE)
	{
		bool wait = false;
		do
		{
			EnterCriticalSection(&rwcs);
			// acquire lock if
			//   - there are no active writers nor waiting writers
			//
			if (!m_wrcnt && !m_wrwaitcnt)
			{
				if(wait)
				{
					--m_rdwaitcnt;
					wait = false;
				}
				++m_rdcnt;
			}
			else
			{
				if(!wait)
				{
					++m_rdwaitcnt;
					wait = true;
				}
				// always reset the event to avoid 100% CPU usage
				ResetEvent(rdEv);
			}
			LeaveCriticalSection(&rwcs);

			if (wait)
			{
				if (WaitForSingleObject(rdEv,timeoms)!=WAIT_OBJECT_0)
				{
					EnterCriticalSection(&rwcs);
					--m_rdwaitcnt;
					SetEvent(rdEv);
					SetEvent(wrEv);
					LeaveCriticalSection(&rwcs);
					return false;
				}
			}
		}while(wait);
		return true;
	}

	void unlock_read()
	{
		EnterCriticalSection(&rwcs);
		--m_rdcnt;
		// always release waiting threads (do not check for m_rdcnt == 0)
		if(m_wrwaitcnt)
		SetEvent(wrEv);
		else if(m_rdwaitcnt)
		SetEvent(rdEv);
		LeaveCriticalSection(&rwcs);
	}

	bool lock_write(DWORD timeoms=INFINITE)
	{
		bool wait = false;
		do
		{
			EnterCriticalSection(&rwcs);
			// acquire lock if there are no active readers nor writers
			if (!m_rdcnt && !m_wrcnt)
			{
				if(wait)
				{
					--m_wrwaitcnt;
					wait = false;
				}
				++m_wrcnt;
			}
			else
			{
				if(!wait)
				{
					++m_wrwaitcnt;
					wait = true;
				}
				// always reset the event to avoid 100% CPU usage
				ResetEvent(wrEv);
			}
			LeaveCriticalSection(&rwcs);
			if (wait)
			{
				if (WaitForSingleObject(wrEv, timeoms)!=WAIT_OBJECT_0)
				{
					EnterCriticalSection(&rwcs);
					m_wrwaitcnt--;
					SetEvent(rdEv);
					SetEvent(wrEv);
					LeaveCriticalSection(&rwcs);
					return false;
				}
			}

		}while(wait);
		return true;
	}

	void unlock_write()
	{
		EnterCriticalSection(&rwcs);
		--m_wrcnt;
		if(m_wrwaitcnt)
		SetEvent(wrEv);
		else if(m_rdwaitcnt)
		SetEvent(rdEv);
		LeaveCriticalSection(&rwcs);
	}
#elif defined(__LINUX)
public:
	pthread_rwlock_t m_lock;
	CRWLock()
	{
		pthread_rwlock_init(&m_lock, 0);
	}

	~CRWLock()
	{
		pthread_rwlock_destroy(&m_lock);
	}

	bool lock_read()
	{
		return 0 == pthread_rwlock_rdlock(&m_lock);
	}

	bool lock_write()
	{
		return 0 == pthread_rwlock_wrlock(&m_lock);
	}

	void unlock_write()
	{
		pthread_rwlock_unlock(&m_lock);
	}

	void unlock_read()
	{
		pthread_rwlock_unlock(&m_lock);
	}
#endif
};

class CSemaphore
{
#ifdef __WINDOWS
private:
	HANDLE m_semaphore;
public:
	CSemaphore(int nMaxCount=10, int nInitialcount=-1)
	{
		m_semaphore = ::CreateSemaphore(0, nInitialcount, nMaxCount, 0);
	}
	virtual ~CSemaphore()
	{
		::CloseHandle(m_semaphore);
	}
	virtual bool wait (int ms=-1)
	{
		return WAIT_OBJECT_0 == WaitForSingleObject(m_semaphore, ms<0 ? INFINITE : ms );
	}
	virtual bool release()
	{
		return ::ReleaseSemaphore(m_semaphore, 1, 0) > 0;
	}
#else
private:
	sem_t m_semaphore;
public:
	CSemaphore(int nMaxCount = 10, int nInitialcount = 0)
	{
		nInitialcount = nInitialcount < nMaxCount ? nInitialcount : nMaxCount;
		sem_init(&m_semaphore, 0, nInitialcount);
	}
	virtual ~CSemaphore()
	{
		sem_destroy(&m_semaphore);
	}
	virtual bool wait (int ms = -1)
	{
		int rc = 0;
		if (ms < 0)
		{
			while ((rc = sem_wait (&m_semaphore)) == -1 && errno == EINTR)
				continue;
		}
		else if (ms == 0)
		{
			while ((rc = sem_trywait (&m_semaphore)) == -1 && errno == EINTR)
				continue;
		}
		else
		{
			timespec ts;
			ts.tv_sec = ms / 1000;
			ts.tv_nsec = ms % 1000 * 1000000;
			if (ts.tv_nsec >= 1000000000)
			{
				ts.tv_nsec = ts.tv_nsec / 1000000000;
				ts.tv_sec++;
			}
			while ((rc = sem_timedwait (&m_semaphore, &ts)) == -1 && errno == EINTR)
				continue;
		}
        return true;
	}
	virtual bool release()
	{
		return 0 == sem_post(&m_semaphore);
	}
#endif
	virtual bool post()
	{
		return this->release();
	}
	virtual bool lock()
	{
		return this->wait (-1);
	}
	virtual bool unlock()
	{
		return this->release();
	}
};

//template <class  T=Mutex>
class CAutoGuard
{
public:
	Mutex *m_lock;

	CAutoGuard(Mutex& lock) : m_lock(&lock)
	{
		m_lock->lock();
	}
	CAutoGuard(Mutex* lock) : m_lock(lock)
	{
		if(m_lock != NULL)
		{
		    m_lock->lock();
		}
	}
	~CAutoGuard()
	{
        if(m_lock != NULL)
		    m_lock->unlock();
	}
};

class CAutoGuardRD
{
public:
	CRWLock& m_lock;
	bool m_locked;
	CAutoGuardRD(CRWLock& lock) : m_lock(lock), m_locked(0)
	{
		m_locked = m_lock.lock_read();
	}
	~CAutoGuardRD()
	{
		if (m_locked)
		{
			m_lock.unlock_read();
		}
	}
};

class CAutoGuardWR
{
	CRWLock& m_lock;
	bool m_locked;
	CAutoGuardWR(CRWLock& lock) : m_lock(lock), m_locked(0)
	{
		m_locked = m_lock.lock_write();
	}
	~CAutoGuardWR()
	{
		if (m_locked)
			m_lock.unlock_write();
	}
};

}
#endif

