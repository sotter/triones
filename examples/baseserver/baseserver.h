/******************************************************
 *   FileName: baseserver.h
 *     Author: triones  2014-10-8
 *Description:
 *******************************************************/

#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "net/cnet.h"

namespace triones
{

class BaseServer : public BaseService
{
public:

	BaseServer();
	virtual ~BaseServer();

	void start(const char *host, int thread = 1)
	{
		init(thread);
		_transport->start();
		_transport->listen(host, _stream, this);
	}

	virtual void handle_queue_packet(IOComponent *ioc, Packet *packet)
	{

//		printf("receive from %s len %d : %s \n",
//				ioc->getSocket()->getAddr().c_str(),
//				packet->getDataLen(),
//				packet->getData());

		Packet *pack = new Packet;
		pack->writeBytes(_send_buffer, sizeof(_send_buffer));
		if (!ioc->postPacket(pack))
		{
			delete pack;
			pack = NULL;
		}
	}

private:

	TransProtocol *_tp;
};

} /* namespace triones */
#endif /* BASESERVER_H_ */
