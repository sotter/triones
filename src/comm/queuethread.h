/**
 * author: Triones
 * date  : 2014-08-21
 */

#ifndef __TRIONES_QUEUETHREAD_H__
#define __TRIONES_QUEUETHREAD_H__

#include "thread.h"
#include "cond.h"

namespace triones
{

class IPackQueue
{
public:
	virtual ~IPackQueue()
	{
	}
	virtual bool push(void *packet) = 0;
	virtual void * pop(void) = 0;
	virtual void free(void *packet) = 0;
	virtual int size(void) = 0;
};

class IQueueHandler
{
public:
	virtual ~IQueueHandler()
	{

	}
	virtual void handle_queue(void *packet) = 0;
};

class QueueThread: public Runnable
{
public:
	QueueThread(IPackQueue *queue, IQueueHandler *handler);

	virtual ~QueueThread();
	bool init(int thread);
	void stop(void);
	bool push(void *packet);

public:
	void run(void *param);

private:
	IPackQueue *_queue;
	IQueueHandler *_handler;
	triones::CThreadCond _mutex;
	ThreadManager _threadmgr;
	bool _stop;
};

} // namespace triones

#endif // #ifndef __TRIONES_QUEUETHREAD_H__
