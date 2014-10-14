/******************************************************
 *   FileName: baseclient.h
 *     Author: triones  2014-10-8
 *Description:
 *******************************************************/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "net/cnet.h"

namespace triones
{

class BaseClient : public BaseService
{
public:
	BaseClient();
	virtual ~BaseClient();

	void start(const char *host, int thread)
	{
		init(thread);
		_transport->start();
		TCPComponent *conn = connect(host, _stream, true);
		if(conn == NULL)
			return;

		Packet *pack = new Packet;
		pack->writeBytes("NOOP \r\n", 7);
		if (!conn->postPacket(pack))
		{
			delete pack;
			pack = NULL;
		}
	}

	virtual void handle_queue_packet(IOComponent *ioc, Packet *packet)
	{
//		printf("receive from %s len %d : %s \n", ioc->getSocket()->getAddr().c_str(),
//		        packet->getDataLen(), packet->getData());

		Packet *pack = new Packet;
		pack->writeBytes("NOOP \r\n", 7);
		if (!ioc->postPacket(pack))
		{
			delete pack;
			pack = NULL;
		}
	}


private:

};

} /* namespace triones */
#endif /* BASECLIENT_H_ */
