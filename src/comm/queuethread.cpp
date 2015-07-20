/**
 * author: Triones
 * date  : 2014-08-21*
 * desc  : 数据处理队列线程对象
 */
#include "mutex.h"
#include "sem.h"
#include "thread.h"
#include "cond.h"
#include "queuethread.h"

namespace triones
{

QueueThread::QueueThread(IPackQueue *queue, IQueueHandler *handler)
		: _queue(queue), _handler(handler), _stop(false)
{

}

QueueThread::~QueueThread()
{
	stop();
	_threadmgr.stop();
}

// 初始化
bool QueueThread::init(int thread)
{
	if (!_threadmgr.init(thread, NULL, this))
	{
		return false;
	}
	_threadmgr.start();
	return true;
}

// 停止
void QueueThread::stop(void)
{
	_mutex.lock();
	_stop = true;
	_mutex.broadcast();
	_mutex.unlock();
}

// 存放数据
bool QueueThread::push(void *packet)
{
	if (packet == NULL) return false;

	if (_stop)
	{
		//方法类，不加锁
		_queue->free(packet);
		return true;
	}

	_mutex.lock();
	if (!_queue->push(packet))
	{
		_mutex.unlock();
		return false;
	}
	_mutex.signal();
	_mutex.unlock();

	return true;
}

// 线程运行接口对象
void QueueThread::run(void *param)
{
	(void) param; // make compiler happy
	void *p = NULL;

	_mutex.lock();
	while (!_stop)
	{
		while (_stop == false && _queue->size() <= 0)
		{
			_mutex.wait();
		}

		if (_stop)
		{
			break;
		}

		p = _queue->pop();
		_mutex.unlock();

		if (p != NULL)
		{
			_handler->handle_queue(p);
		}

		_mutex.lock();
	}
	//stop完毕后强行将所有的数据都释放掉；
	while ((p = _queue->pop()) != NULL)
	{
		_queue->free(p);
	}

	_mutex.unlock();
}

} // namespace triones

