/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 鏁版嵁澶勭悊闃熷垪绾跨▼瀵硅薄
 */

#ifndef __TRIONES_QUEUETHREAD_H__
#define __TRIONES_QUEUETHREAD_H__

#include "thread.h"
#include "cond.h"

namespace triones
{

// 鏁版嵁鍖呴槦鍒�
class IPackQueue
{
public:
	virtual ~IPackQueue()
	{
	}
	// 瀛樻斁鏁版嵁
	virtual bool push(void *packet) = 0;
	// 寮瑰嚭鏁版嵁
	virtual void * pop(void) = 0;
	// 閲婃斁鏁版嵁
	virtual void free(void *packet) = 0;
	// 鍙栧緱褰撳墠闃熷垪闀垮害
	virtual int size(void) = 0;
};

// 鏁版嵁浼犻�鎺ュ彛
class IQueueHandler
{
public:
	virtual ~IQueueHandler()
	{

	}
	// 浜ゅ嚭鏁版嵁鍥炶皟鎺ュ彛
	virtual void handle_queue(void *packet) = 0;
};

// 绾跨▼鏁版嵁澶勭悊闃熷垪
class QueueThread: public Runnable
{
public:
	QueueThread(IPackQueue *queue, IQueueHandler *handler);

	virtual ~QueueThread();
	//  鍒濆鍖�
	bool init(int thread);
	// 鍋滄
	void stop(void);
	// 瀛樻斁鏁版嵁
	bool push(void *packet);

public:
	// 绾跨▼杩愯鎺ュ彛瀵硅薄
	void run(void *param);

private:
	// 鏁版嵁瀛樻斁闃熷垪
	IPackQueue *_queue;
	// 鏁版嵁鍥炶皟鎺ュ彛
	IQueueHandler *_handler;
	//
	triones::CThreadCond _mutex;
	// 绾跨▼绠＄悊瀵硅薄
	ThreadManager _threadmgr;
	// 鏄惁鍒濆鍖栨暟鎹�
	bool _stop;
};

} // namespace triones

#endif // #ifndef __TRIONES_QUEUETHREAD_H__
