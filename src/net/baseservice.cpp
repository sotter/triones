/******************************************************
 *   FileName: baseservice.cpp
 *     Author: triones  2014-9-26
 *Description:
 *******************************************************/

#include "baseservice.h"

namespace triones
{
BaseService::BaseService()
{
	_packqueue   = new triones::CDataQueue<triones::BasePacket>(MAXQUEUE_LENGTH);
	_queue_thread = new QueueThread(_packqueue, this);
	_transport = new Transport();
	_stream = __trans_protocol.get(TPROTOCOL_TEXT);

	memset(_send_buffer, 0x31, sizeof(_send_buffer) - 2);
	_send_buffer[sizeof(_send_buffer) - 2] = 0x0d;
	_send_buffer[sizeof(_send_buffer) - 1] = 0x0a;

}

BaseService::~BaseService()
{
	// TODO Auto-generated destructor stub
}

bool BaseService:: init(int thread_num /* = 1 */ ,
		int transproto /* = TPROTOCOL_TEXT*/ )
{
	//实际上在这里面线程就已经启动了，而不用start
	if(! _queue_thread->init(thread_num))
		return false;

	_stream = __trans_protocol.get(transproto);
	return _stream != NULL;
}

TCPComponent *BaseService::connect(const char *spec, triones::TransProtocol *streamer, bool autoReconn)
{
	TCPComponent *tc = _transport->connect(spec, streamer, autoReconn);
	if(tc != NULL)
	{
		tc->setServerAdapter(this);
	}
	return tc;
}

//IServerAdapter的回调函数，处理单个packet的情况。直接加入业务队列中，这样就做到了网络层和业务层的剥离；
bool BaseService::handlePacket(IOComponent *connection, Packet *packet)
{
	__INTO_FUN__

//	BasePacket *base_pack = new BasePacket;
//	base_pack->_ioc = connection;
//	base_pack->_packet = packet;
//
//	/* *************************
//	 * （1）BasePacket的析构函数不会释放connectiong和packet，connection由网络层管理释放，packet是由handle_queue主导释放的；
//	 * （2）push失败时，packet是由这里负责释放的；
//	 * （3）如果push失败时，queue_thread是不负责释放base_pack的，需由这里进行释放。
//	 * *************************/
//	if(! _queue_thread->push((void*)base_pack))
//	{
//		delete base_pack->_packet;
//		delete base_pack;
//	}

	UNUSED(packet);
	static int count = 3;

	if (count++ > 0)
	{
		if(count > 10000)
		{
			count = 1;
			printf("#########################SEND %lu count %d\n", sizeof(_send_buffer), count);
		}

		Packet *pack1 = new Packet;
		pack1->writeBytes(_send_buffer, sizeof(_send_buffer));
		if (!connection->postPacket(pack1))
		{
			delete pack1;
			pack1 = NULL;
		}
	}

	return true;
}

void BaseService::handle_queue(void *packet)
{
	__INTO_FUN__

	BasePacket *base_pack = (BasePacket*)packet;

	handle_queue_packet(base_pack->_ioc, base_pack->_packet);

	//在这里讲packet释放掉，base_pack的释放由queuethread来完成
	delete base_pack->_packet;
}

//处理有同步业务层的处理，子类的service来实现
void BaseService::handle_queue_packet(IOComponent *ioc, Packet *packet)
{
	__INTO_FUN__
	printf("handle pack %s, %s", ioc->getSocket()->getAddr().c_str(), packet->_pdata);
	return;
}


} /* namespace triones */
