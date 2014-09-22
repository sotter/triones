/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 数据处理队列线程对象
 */

#ifndef __TRIONES_QUEUETHREAD_H__
#define __TRIONES_QUEUETHREAD_H__

#include "mutex.h"
#include "sem.h"
#include "thread.h"

namespace triones
{

// 数据包队列
class IPackQueue
{
public:
	virtual ~IPackQueue()
	{
	}
	// 存放数据
	virtual bool push(void *packet) = 0;
	// 弹出数据
	virtual void * pop(void) = 0;
	// 释放数据
	virtual void free(void *packet) = 0;
	// 取得当前队列长度
	virtual int size(void) = 0;
};

// 数据传送接口
class IQueueHandler
{
public:
	virtual ~IQueueHandler()
	{
	}
	// 交出数据回调接口
	virtual void handle_queue(void *packet) = 0;
};

// 线程数据处理队列
class QueueThread: public Runnable
{
public:
	QueueThread(IPackQueue *queue, IQueueHandler *handler);
	virtual ~QueueThread();
	//  初始化
	bool init(int thread);
	// 停止
	void stop(void);
	// 存放数据
	bool push(void *packet);

public:
	// 线程运行接口对象
	void run(void *param);
	// 处理数据
	void process(void);

private:
	// 数据存放队列
	IPackQueue *_queue;
	// 数据回调接口
	IQueueHandler *_handler;
	// 信号管理对象
	Mutex _mutex;
	//任务队列的信号量管理
	Sem _sem;
	// 线程管理对象
	ThreadManager _threadmgr;
	// 是否初始化数据
	bool _inited;
};

} // namespace triones

#endif // #ifndef __TRIONES_QUEUETHREAD_H__
