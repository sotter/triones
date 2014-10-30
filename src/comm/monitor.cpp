/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 多线程使用 Monitor对象需要配合Synchronized来使用，如果不进行同步而存在多线程数据错乱情况
 */

#include <assert.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include "mutex.h"
#include "util.h"
#include "exception.h"
#include "monitor.h"

namespace triones
{

/**
 * Monitor implementation using the POSIX pthread library
 */
class Monitor::Impl
{

public:

	Impl() :
			_mutex(NULL), _cond_initialized(false), _cond_notifyend(false)
	{
		init(&_owned_mutex);
	}

	Impl(Mutex* mutex) :
			_mutex(NULL), _cond_initialized(false), _cond_notifyend(false)
	{
		init(mutex);
	}

	Impl(Monitor* monitor) :
			_mutex(NULL), _cond_initialized(false), _cond_notifyend(false)
	{
		init(&(monitor->mutex()));
	}

	~Impl()
	{
		cleanup();
	}

	Mutex& mutex()
	{
		return *_mutex;
	}
	void lock()
	{
		mutex().lock();
	}
	void unlock()
	{
		mutex().unlock();
	}

	void wait(int64_t timeout) const
	{
		// 如果已经收到停止的指令直接退出
		if (_cond_notifyend)
			return;

		assert(_mutex);
		pthread_mutex_t* mutex_impl =
				reinterpret_cast<pthread_mutex_t*>(_mutex->get_impl());
		assert(mutex_impl);

		// XXX Need to assert that caller owns mutex
		assert(timeout >= 0LL);
		if (timeout == 0LL)
		{
			int iret = pthread_cond_wait(&_pthread_cond, mutex_impl);
			assert(iret == 0);
		}
		else
		{
			struct timespec abstime;
			int64_t now = Util::current_time();
#ifdef _USE_SECOND
			Util::to_timespec(abstime, now , timeout );
#else
			Util::to_timespec(abstime, now + timeout);
#endif
			int result = pthread_cond_timedwait(&_pthread_cond, mutex_impl, &abstime);
			if (result == ETIMEDOUT)
			{
				// pthread_cond_timedwait has been observed to return early on
				// various platforms, so comment out this assert.
#ifdef _XDEBUG
				printf( "begin time %d, time out %d, current time %d\n" , now, timeout, Util::currentTime() );
#endif
				// assert(Util::currentTime() >= (now + timeout));
				// throw TimedOutException();
			}
		}
	}

	void notify()
	{
		// XXX Need to assert that caller owns mutex
		int iret = pthread_cond_signal(&_pthread_cond);
		assert(iret == 0);
	}

	void notify_all()
	{
		// XXX Need to assert that caller owns mutex
		int iret = pthread_cond_broadcast(&_pthread_cond);
		assert(iret == 0);
	}

	void notify_end()
	{
		// 设置结束标识
		_cond_notifyend = true;
		// XXX Need to assert that caller owns mutex
		int iret = pthread_cond_broadcast(&_pthread_cond);
		assert(iret == 0);
	}

private:

	void init(Mutex* mutex)
	{
		_mutex = mutex;

		if (pthread_cond_init(&_pthread_cond, NULL) == 0)
		{
			_cond_initialized = true;
		}

		if (!_cond_initialized)
		{
			cleanup();
			throw SystemResourceException();
		}
	}

	void cleanup()
	{
		if (_cond_initialized)
		{
			_cond_initialized = false;
			int iret = pthread_cond_destroy(&_pthread_cond);
			assert(iret == 0);
		}
	}

	Mutex _owned_mutex;
	Mutex* _mutex;

	mutable pthread_cond_t _pthread_cond;
	mutable bool _cond_initialized;
	mutable bool _cond_notifyend;
};

Monitor::Monitor() :
		_impl(new Monitor::Impl())
{
}
Monitor::Monitor(Mutex* mutex) :
		_impl(new Monitor::Impl(mutex))
{
}
Monitor::Monitor(Monitor* monitor) :
		_impl(new Monitor::Impl(monitor))
{
}

Monitor::~Monitor()
{
	delete _impl;
}

Mutex& Monitor::mutex() const
{
	return _impl->mutex();
}

void Monitor::lock() const
{
	_impl->lock();
}

void Monitor::unlock() const
{
	_impl->unlock();
}

void Monitor::wait(int64_t timeout) const
{
	_impl->wait(timeout);
}

void Monitor::notify() const
{
	_impl->notify();
}

void Monitor::notify_all() const
{
	_impl->notify_all();
}

void Monitor::notify_end() const
{
	_impl->notify_end();
}

Synchronized::Synchronized(const Monitor* monitor)
:_g(monitor->mutex())
{
}

Synchronized::Synchronized(const Monitor& monitor)
:_g(monitor.mutex())
{
}

} // namespace triones
