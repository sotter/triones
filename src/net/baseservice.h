/******************************************************
 *   FileName: baseservice.h
 *     Author: triones  2014-9-26
 *Description: （1）BaseService进行封装，封装为一个已经增加一个异步队列的transport
 *Description: （2）分包器的配置粒度为：统一个server accept的client共同一个分包器；
 *Description: 	  主动发起连接的client，可以配置自己的分包器。
 *Description:
 *******************************************************/

#ifndef BASESERVICE_H_
#define BASESERVICE_H_

#include "cnet.h"
#include "../pack/pack.h"
#include "../comm/queuethread.h"
#include "../comm/dataqueue.h"
#include "../pack/tprotocol.h"

using namespace triones;

namespace triones
{

class BaseService: public IServerAdapter, public IQueueHandler
{
public:

#define MAXQUEUE_LENGTH        102400    // 最大队列长度
	//queuethread 异步队列，业务线程的处理数据, thread_num
	bool init(int thread_num = 1);

	//spec:连接地址 tcp:127.0.0.1:7008, streamer分包器
	IOComponent* connect(const char *spec, int streamer, bool autoReconn = false);

	//spec:连接地址 tcp:127.0.0.1:7008, streamer分包器
	IOComponent* listen(const char *spec, int streamer);

	//处理有同步业务层的处理，子类的service来实现
	virtual void handle_packet(IOComponent *ioc, Packet *packet);

	//上层业务销毁的回调函数，非必须实现；
	virtual bool destroy_service();

	//停止线程，释放相关资源；
	bool destroy();

public:

	BaseService();

	virtual ~BaseService();

	//IServerAdapter的回调函数，处理单个packet的情况。直接加入业务队列中，这样就做到了网络层和业务层的剥离；
	virtual bool syn_handle_packet(IOComponent *connection, Packet *packet);

	//QueueThread异步队列的异步回调函数
	virtual void handle_queue(void *packet);

protected:
	//网络模型， 设置成protected, 子类有可能会调用
	Transport* _transport;

	//做性能测试使用
	char _send_buffer[16];

private:
	//是否已经经过初始化
	bool _inited;

	//异步队列， 供_queue_thread使用
	CDataQueue<Packet> * _packqueue;

	//packet线程队列
	QueueThread *_queue_thread;
};

} /* namespace triones */
#endif /* BASESERVICE_H_ */
