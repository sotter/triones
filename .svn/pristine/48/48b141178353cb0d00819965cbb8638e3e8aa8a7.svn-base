/**
 * author: Triones
 * date  : 2014-08-21*
 * desc  : 数据处理队列线程对象
 */
#include "mutex.h"
#include "sem.h"
#include "thread.h"
#include "queuethread.h"

namespace triones
{

QueueThread::QueueThread(IPackQueue *queue, IQueueHandler *handler)
:_queue(queue), _handler(handler), _inited(false)
{

}

QueueThread::~QueueThread()
{
	stop();
}

// 初始化
bool QueueThread::init(int thread)
{
	if (!_threadmgr.init(thread, NULL, this))
	{
		return false;
	}
	_inited = true;
	_sem.init();
	_threadmgr.start();
	return true;
}

// 停止
void QueueThread::stop( void )
{
	if ( ! _inited )
		return ;
	_inited = false ;
	_threadmgr.stop() ;
}

// 存放数据
bool QueueThread::push( void *packet )
{
	_mutex.lock() ;
	if ( ! _queue->push( packet ) ) {
		_mutex.unlock() ;
		return false ;
	}
	_mutex.unlock() ;

	_sem.post();

	return true ;
}

// 处理数据
void QueueThread::process(void)
{
	_sem.wait();

	void *p = NULL;

	// 从数据队列中取数据
	_mutex.lock();
	p = _queue->pop();
	if (p == NULL)
	{
		_mutex.unlock();
		return;
	}
	_mutex.unlock();

	// 回调数据处理对象
	_handler->handle_queue(p);

	// 释放数据
	_queue->free(p);
}

// 线程运行接口对象
void QueueThread::run(void *param)
{
	(void)param; // make compiler happy

	while (_inited)
	{
		process();
	}
}

} // namespace triones

