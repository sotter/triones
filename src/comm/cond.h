/******************************************************
 *   FileName: cond.h
 *     Author: triones  2014-10-16
 *Description: 移植淘宝的代码
 *******************************************************/

/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#ifndef COND_H_
#define COND_H_

#include <assert.h>
#include <sys/time.h>

namespace triones
{

/*
 * author cjxrobot
 *
 * Linux线程锁
 */

/**
 * @brief linux线程锁互斥锁简单封装
 */
class CThreadMutex
{

public:
	/*
	 * 构造函数
	 */
	CThreadMutex()
	{
		//assert(pthread_mutex_init(&_mutex, NULL) == 0);
		const int iRet = pthread_mutex_init(&_mutex, NULL);
		(void) iRet;
		assert(iRet == 0);
	}

	/*
	 * 析造函数
	 */
	~CThreadMutex()
	{
		pthread_mutex_destroy(&_mutex);
	}

	/**
	 * 加锁
	 */

	void lock()
	{
		pthread_mutex_lock(&_mutex);
	}

	/**
	 * trylock加锁
	 */

	int trylock()
	{
		return pthread_mutex_trylock(&_mutex);
	}

	/**
	 * 解锁
	 */
	void unlock()
	{
		pthread_mutex_unlock(&_mutex);
	}

protected:

	pthread_mutex_t _mutex;
};

/**
 * @brief 线程的Guard
 */
class CThreadGuard
{
public:
	CThreadGuard(CThreadMutex *mutex)
	{
		_mutex = NULL;
		if (mutex)
		{
			_mutex = mutex;
			_mutex->lock();
		}
	}
	~CThreadGuard()
	{
		if (_mutex)
		{
			_mutex->unlock();
		}
	}
private:
	CThreadMutex *_mutex;
};

/**
 * @brief  Linux线程条件变量
 */
class CThreadCond: public CThreadMutex
{

public:

	/**
	 * 构造函数
	 */
	CThreadCond()
	{
		pthread_cond_init(&_cond, NULL);
	}

	/**
	 * 析造函数
	 */
	~CThreadCond()
	{
		pthread_cond_destroy(&_cond);
	}

	/**
	 * 等待信号
	 *
	 * @param  milliseconds  等待超时间(单位:ms), 0 = 永久等待，
	 */
	bool wait(int milliseconds = 0)
	{
		bool ret = true;

		if (milliseconds == 0)
		{ // 永久等待
			pthread_cond_wait(&_cond, &_mutex);
		}
		else
		{

			struct timeval curtime;

			struct timespec abstime;
			gettimeofday(&curtime, NULL);

			int64_t us = (static_cast<int64_t>(curtime.tv_sec) * static_cast<int64_t>(1000000)
			        + static_cast<int64_t>(curtime.tv_usec)
			        + static_cast<int64_t>(milliseconds) * static_cast<int64_t>(1000));

			abstime.tv_sec = static_cast<int>(us / static_cast<int64_t>(1000000));
			abstime.tv_nsec = static_cast<int>(us % static_cast<int64_t>(1000000)) * 1000;
			ret = (pthread_cond_timedwait(&_cond, &_mutex, &abstime) == 0);
		}

		return ret;
	}

	/**
	 * 唤醒一个
	 */
	void signal()
	{
		pthread_cond_signal(&_cond);
	}

	/**
	 * 唤醒所有
	 */
	void broadcast()
	{
		pthread_cond_broadcast(&_cond);
	}

private:
	pthread_cond_t _cond;
};

}

#endif /* COND_H_ */
