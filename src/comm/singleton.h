/**
 * author: Triones
 * date  : 2014-08-20
 */
#ifndef __TRIONES_SINGLETON_H__
#define __TRIONES_SINGLETON_H__

#include <memory>

namespace triones
{

//是否启用锁
#define  __USE_THREAD__

#ifdef __USE_THREAD__
#include <pthread.h>

#define THREAD_LOCK(thread_mutex)     thread_mutex.Lock()
#define THREAD_UNLOCK(thread_mutex)   thread_mutex.UnLock()
#else
#define THREAD_LOCK(thread_mutex)
#define THREAD_UNLOCK(thread_mutex)
#endif

#define DECLARE_SINGLETON_CLASS( type ) \
	friend class std::auto_ptr< type >;  \
friend class Singleton< type >;


class ThreadMutex
{
public:
#ifndef __USE_THREAD__
	ThreadMutex(){};
	~ThreadMutex(){};
	void Lock(){};
	void UnLock(){};
#else
	ThreadMutex()
	{
#ifdef WIN32
		InitializeCriticalSection(&_thread_mutex);
#else
		pthread_mutex_init(&_thread_mutex, NULL);
#endif
	}
	~ThreadMutex()
	{
#ifdef WIN32
		DeleteCriticalSection(&_thread_mutex);
#else
		pthread_mutex_destroy(&_thread_mutex);
#endif
	}
	void Lock()
	{
#if defined(WIN32)
		EnterCriticalSection(&_thread_mutex);
#else
		if( pthread_mutex_lock(&_thread_mutex) != 0 )
			perror(__FILE__"Error: Lock fail!\n");
#endif
	}	
	void UnLock()
	{
#if defined(WIN32)
		LeaveCriticalSection(&_thread_mutex);
#else
		if ( pthread_mutex_unlock(&_thread_mutex) != 0 )
			perror(__FILE__"Error: Unlock fail!\n");
#endif
	}
#endif

private:
#ifdef __USE_THREAD__
#if defined(WIN32)
	CRITICAL_SECTION	_thread_mutex;
#else
	pthread_mutex_t		_thread_mutex;
#endif
#endif
};

template <class T>
class Singleton
{
public:
	static T* instance();

protected:
	Singleton()
	{
	}
	virtual ~Singleton()
	{
	}

protected:
	static std::auto_ptr<T> _instance;
	static ThreadMutex _thread_mutex;
};

template <class T>
std::auto_ptr<T> Singleton<T>::_instance;

template <class T>
ThreadMutex Singleton<T>::_thread_mutex;

template <class T>
inline T* Singleton<T>::instance()
{
	//Double-Checked Locking
	if (0 == _instance.get())
	{
		THREAD_LOCK(_thread_mutex);
		if (0 == _instance.get())
		{
			_instance.reset(::new T);
		}
		THREAD_UNLOCK(_thread_mutex);
	}

	return _instance.get();
}

}  // namespace triones

#endif // #ifndef __TRIONES_SINGLETON_H__

