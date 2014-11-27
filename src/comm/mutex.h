/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : source from apache some edited
 */

#ifndef __TRIONES_MUTEX_H__
#define __TRIONES_MUTEX_H__

#include <stdlib.h>
#include <stdint.h>

namespace triones
{

class Mutex
{
public:
	typedef void (*Initializer)(void*);

	Mutex(Initializer init = DEFAULT_INITIALIZER);
	virtual ~Mutex();
	virtual void lock() const;
	virtual bool try_lock() const;
	virtual bool timed_lock(int64_t milliseconds) const;
	virtual void unlock() const;

	void* get_impl() const;

	static void DEFAULT_INITIALIZER(void*);
	static void ADAPTIVE_INITIALIZER(void*);
	static void RECURSIVE_INITIALIZER(void*);

private:

	class impl;
	impl *_impl;
};

class RWMutex
{
public:
	RWMutex();
	virtual ~RWMutex();

	// these get the lock and block until it is done successfully
	virtual void rdlock() const;
	virtual void wrlock() const;

	// these attempt to get the lock, returning false immediately if they fail
	virtual bool try_rdlock() const;
	virtual bool try_wrlock() const;

	// this releases both read and write locks
	virtual void unlock() const;

private:

	class impl;
	impl *_impl;
};

class Guard
{
public:
	Guard(const Mutex& value, int64_t timeout = 0)
			: _mutex(&value)
	{
		if (timeout == 0)
		{
			value.lock();
		}
		else if (timeout < 0)
		{
			if (!value.try_lock())
			{
				_mutex = NULL;
			}
		}
		else
		{
			if (!value.timed_lock(timeout))
			{
				_mutex = NULL;
			}
		}
	}

	~Guard()
	{
		if (_mutex)
		{
			_mutex->unlock();
		}
	}

	operator bool() const
	{
		return (_mutex != NULL);
	}

private:
	const Mutex* _mutex;
};

enum RWGuardType
{
	RW_READ = 0, RW_WRITE = 1,
};

class RWGuard
{
public:
	RWGuard(const RWMutex& value, bool write = false)
			: _rw_mutex(value)
	{
		if (write)
		{
			_rw_mutex.wrlock();
		}
		else
		{
			_rw_mutex.rdlock();
		}
	}

	RWGuard(const RWMutex& value, RWGuardType type)
			: _rw_mutex(value)
	{
		if (type == RW_WRITE)
		{
			_rw_mutex.wrlock();
		}
		else
		{
			_rw_mutex.rdlock();
		}
	}

	~RWGuard()
	{
		_rw_mutex.unlock();
	}
private:
	const RWMutex& _rw_mutex;
};
}  // namespace triones

#endif // #ifndef __TRIONES_MUTEX_H__
