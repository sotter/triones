/******************************************************
 *   FileName: baseclient.h
 *     Author: triones  2014-10-8
 *Description:
 *******************************************************/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "net/cnet.h"
#include "pack/tprotocol.h"

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

		IOComponent *conn = connect(host, triones::TPROTOCOL_TEXT, true);
		if(conn == NULL)
			return;

		Packet *pack = new Packet;
		pack->writeBytes(_send_buffer, sizeof(_send_buffer));
		if (!conn->postPacket(pack))
		{
			delete pack;
			pack = NULL;
		}
	}

	virtual void handle_packet(IOComponent *ioc, Packet *packet)
	{
		if (packet->get_type() == IServerAdapter::CMD_DISCONN_PACKET)
		{
			printf("receive disconnection %s \n", ioc->getSocket()->getAddr().c_str());
		}
		else if (packet->get_type() == IServerAdapter::CMD_DATA_PACKET)
		{
			printf("receive from %s len %d : %s \n", ioc->getSocket()->getAddr().c_str(),
			        packet->getDataLen(), packet->getData());
		}

//		Packet *pack = new Packet;
//		pack->writeBytes(_send_buffer, sizeof(_send_buffer));
//		if (!ioc->postPacket(pack))
//		{
//			delete pack;
//			pack = NULL;
//		}
	}


private:

};

} /* namespace triones */
#endif /* BASECLIENT_H_ */
