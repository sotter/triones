/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : source from apache some edited
 */

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "mutex.h"
#include "util.h"

namespace triones
{

/**
 * Implementation of Mutex class using POSIX mutex
 */
class Mutex::impl
{
public:
	impl(Initializer init) :
			_initialized(false)
	{
		init(&_pthread_mutex);
		_initialized = true;
	}

	~impl()
	{
		if (_initialized)
		{
			_initialized = false;
			int ret = pthread_mutex_destroy(&_pthread_mutex);
			assert(ret == 0);
		}
	}

	void lock() const
	{
		pthread_mutex_lock(&_pthread_mutex);
	}

	bool try_lock() const
	{
		return (0 == pthread_mutex_trylock(&_pthread_mutex));
	}

	bool timed_lock(int64_t milliseconds) const
	{
#if defined(_POSIX_TIMEOUTS) && _POSIX_TIMEOUTS >= 200112L
		PROFILE_MUTEX_START_LOCK();

		struct timespec ts;
		Util::toTimespec(ts, milliseconds);
		int ret = pthread_mutex_timedlock(&_pthread_mutex, &ts);
		if (ret == 0)
		{
			PROFILE_MUTEX_LOCKED();
			return true;
		}

		PROFILE_MUTEX_NOT_LOCKED();
		return false;
#else
		(void)milliseconds; // make compiler happy
		// If pthread_mutex_timedlock isn't supported, the safest thing to do
		// is just do a nonblocking trylock.
		return try_lock();
#endif
	}

	void unlock() const
	{
		pthread_mutex_unlock(&_pthread_mutex);
	}

	void* get_impl() const
	{
		return (void*) &_pthread_mutex;
	}

private:
	mutable pthread_mutex_t _pthread_mutex;
	mutable bool _initialized;
};

Mutex::Mutex(Initializer init) :
		_impl(new Mutex::impl(init))
{
}

Mutex::~Mutex()
{
	if (_impl != NULL)
		delete _impl;
}

void* Mutex::get_impl() const
{
	return _impl->get_impl();
}

void Mutex::lock() const
{
	_impl->lock();
}

bool Mutex::try_lock() const
{
	return _impl->try_lock();
}

bool Mutex::timed_lock(int64_t ms) const
{
	return _impl->timed_lock(ms);
}

void Mutex::unlock() const
{
	_impl->unlock();
}

void Mutex::DEFAULT_INITIALIZER(void* arg)
{
	pthread_mutex_t* pthread_mutex = (pthread_mutex_t*) arg;
	int ret = pthread_mutex_init(pthread_mutex, NULL);
	assert(ret == 0);
}

#if defined(PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP) || defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
static void init_with_kind(pthread_mutex_t* mutex, int kind)
{
	pthread_mutexattr_t mutexattr;
	int ret = pthread_mutexattr_init(&mutexattr);
	assert(ret == 0);

	// Apparently, this can fail.  Should we really be aborting?
	ret = pthread_mutexattr_settype(&mutexattr, kind);
	assert(ret == 0);

	ret = pthread_mutex_init(mutex, &mutexattr);
	assert(ret == 0);

	ret = pthread_mutexattr_destroy(&mutexattr);
	assert(ret == 0);
}
#endif

#ifdef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
void Mutex::ADAPTIVE_INITIALIZER(void* arg)
{
	// From mysql source: mysys/my_thr_init.c
	// Set mutex type to "fast" a.k.a "adaptive"
	//
	// In this case the thread may steal the mutex from some other thread
	// that is waiting for the same mutex. This will save us some
	// context switches but may cause a thread to 'starve forever' while
	// waiting for the mutex (not likely if the code within the mutex is
	// short).
	init_with_kind((pthread_mutex_t*)arg, PTHREAD_MUTEX_ADAPTIVE_NP);
}
#endif

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
void Mutex::RECURSIVE_INITIALIZER(void* arg)
{
	init_with_kind((pthread_mutex_t*)arg, PTHREAD_MUTEX_RECURSIVE_NP);
}
#endif

/**
 * Implementation of ReadWriteMutex class using POSIX rw lock
 */
class RWMutex::impl
{
public:
	impl() :
			_initialized(false)
	{
		int ret = pthread_rwlock_init(&_rw_lock, NULL);
		assert(ret == 0);
		_initialized = true;
	}

	~impl()
	{
		if (_initialized)
		{
			_initialized = false;
			int ret = pthread_rwlock_destroy(&_rw_lock);
			assert(ret == 0);
		}
	}

	void rdlock() const
	{
		pthread_rwlock_rdlock(&_rw_lock);
	}

	void wrlock() const
	{
		pthread_rwlock_wrlock(&_rw_lock);
	}

	bool try_rdlock() const
	{
		return pthread_rwlock_tryrdlock(&_rw_lock);
	}

	bool try_wrlock() const
	{
		return pthread_rwlock_trywrlock(&_rw_lock);
	}

	void unlock() const
	{
		pthread_rwlock_unlock(&_rw_lock);
	}

private:
	mutable pthread_rwlock_t _rw_lock;
	mutable bool _initialized;
};

RWMutex::RWMutex() :
		_impl(new RWMutex::impl())
{
}

RWMutex::~RWMutex()
{
	if (_impl != NULL)
		delete _impl;
}

void RWMutex::rdlock() const
{
	_impl->rdlock();
}

void RWMutex::wrlock() const
{
	_impl->wrlock();
}

bool RWMutex::try_rdlock() const
{
	return _impl->try_rdlock();
}

bool RWMutex::try_wrlock() const
{
	return _impl->try_wrlock();
}

void RWMutex::unlock() const
{
	_impl->unlock();
}

} // namespace triones

