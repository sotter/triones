/******************************************************
 *   FileName: baseservice.cpp
 *     Author: triones  2014-9-26
 *Description:
 *******************************************************/

#include "baseservice.h"

namespace triones
{

BaseService::BaseService()
		: _inited(false)
{
	_packqueue = new triones::CDataQueue<triones::Packet>(MAXQUEUE_LENGTH);
	_queue_thread = new QueueThread(_packqueue, this);
	_transport = new Transport();

	//做乒乓测试时的性能测试数据
	memset(_send_buffer, 0x31, sizeof(_send_buffer) - 2);
	_send_buffer[sizeof(_send_buffer) - 2] = 0x0d;
	_send_buffer[sizeof(_send_buffer) - 1] = 0x0a;
}

BaseService::~BaseService()
{
	destroy();
}

bool BaseService::init(int thread_num /* = 1 */)
{
	//函数可重入，但是后面初始化是thread_num不起作用了。用户如果不关心thread_num
	//可直接调用后面的connect 和 listen
	if (_inited) return true;

	//实际上在这里面线程就已经启动了，而不用start
	if (!_queue_thread->init(thread_num)) return false;

	if (!_transport->start())
	{
		_queue_thread->stop();
		return false;
	}

	_inited = true;

	return true;
}

IOComponent *BaseService::connect(const char *spec, int streamer, bool autoReconn)
{
	if (!init()) return false;

	triones::TransProtocol *tp = __trans_protocol.get(streamer);
	if (tp == NULL) return NULL;

	IOComponent *tc = _transport->connect(spec, tp, autoReconn);
	if (tc != NULL)
	{
		tc->setServerAdapter(this);
	}
	return tc;
}

IOComponent* BaseService::listen(const char *spec, int streamer)
{
	if (!init()) return false;

	triones::TransProtocol *tp = __trans_protocol.get(streamer);
	if (tp == NULL) return NULL;

	TCPAcceptor * acceptor = (TCPAcceptor*) _transport->listen(spec, tp, this);
	if (acceptor != NULL)
	{
		acceptor->setServerAdapter(this);
	}

	return acceptor;
}

//IServerAdapter的回调函数，处理单个packet的情况。直接加入业务队列中，这样就做到了网络层和业务层的剥离；
bool BaseService::synHandlePacket(IOComponent *connection, Packet *packet)
{
	__INTO_FUN__

	packet->_ioc = (void*) connection;

	if (!_queue_thread->push((void*) packet))
	{
		delete packet;
	}

//  下面是直接回调时，乒乓测试的性能测试代码
//	UNUSED(packet);
//	static int count = 3;
//
//	if (count++ > 0)
//	{
//		if(count > 10000)
//		{
//			count = 1;
//			printf("#########################SEND %lu count %d\n", sizeof(_send_buffer), count);
//		}
//
//		Packet *pack1 = new Packet;
//		pack1->writeBytes(_send_buffer, sizeof(_send_buffer));
//		if (!connection->postPacket(pack1))
//		{
//			delete pack1;
//			pack1 = NULL;
//		}
//	}

	return true;
}

//QueueThread异步队列的异步回调函数
void BaseService::handle_queue(void *packet)
{
	__INTO_FUN__
	handle_packet((IOComponent*) (((Packet*) packet)->_ioc), (Packet*) packet);

	//todo:当前的实现为无论handle_packet的处理结果如何，都将packet释放，后面的实现中需要增加返回值来判定是否释放packet
	delete (Packet*)packet;
}

//处理有同步业务层的处理，子类的service来实现
void BaseService::handle_packet(IOComponent *ioc, Packet *packet)
{
	__INTO_FUN__
	printf("BaseService handle pack %s, %s", ioc->getSocket()->getAddr().c_str(), packet->_pdata);
	return;
}

bool BaseService::destroy_service()
{
	return true;
}

bool BaseService::destroy()
{
	if (_transport != NULL)
	{
		//只是transport置了stop标志位，线程还不能保证已经全部结束；
		_transport->stop();
	}

	//放在_transport stop和wait的中间
	destroy_service();

	if (_queue_thread != NULL)
	{
		_queue_thread->stop();
		delete _queue_thread;
		_queue_thread = NULL;
	}

	if (_packqueue != NULL)
	{
		delete _packqueue;
		_packqueue = NULL;
	}

	if (_transport != NULL)
	{
		_transport->wait();
		delete _transport;
		_transport = NULL;
	}

	return true;
}

} /* namespace triones */
