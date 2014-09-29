/******************************************************
 *   FileName: baseservice.h
 *     Author: triones  2014-9-26
 *Description:
 *******************************************************/

#ifndef BASESERVICE_H_
#define BASESERVICE_H_

#include "cnet.h"
#include "../pack/pack.h"
#include "../thread/queuethread.h"
#include "dataqueue.h"

namespace triones
{

class BasePacket
{
public:
	Packet *_packet;
	IOComponent *_ioc;

	BasePacket *_next;
	BasePacket *_pre;
};

class BaseService: public IServerAdapter, public IQueueHandler
{
public:

#define MAXQUEUE_LENGTH        102400    // 最大队列长度

	BaseService();

	virtual ~BaseService();

	//IServerAdapter的回调函数，处理单个packet的情况。直接加入业务队列中，这样就做到了网络层和业务层的剥离；
	virtual bool handlePacket(IOComponent *connection, Packet *packet);

	virtual void handle_queue(void *packet);

	//处理有同步业务层的处理，子类的service来实现
	virtual void handle_queue_packet(IOComponent *ioc, Packet *packet);

private:

	int initialize_network(const char* app_name)
	{
		UNUSED(app_name);
		return 0;
	}

protected:
	//网络模型
	Transport* _transport;
private:

	CDataQueue<BasePacket>	* _packqueue ;
	//packet线程队列
	QueueThread *_queue_thread;
};

} /* namespace triones */
#endif /* BASESERVICE_H_ */
