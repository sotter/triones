/******************************************************
 *   FileName: baseclient.h
 *     Author: triones  2014-10-8
 *Description:
 *******************************************************/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "net/cnet.h"
#include "pack/tprotocol.h"
#include "../../src/comm/comlog.h"

namespace triones
{

class BaseClient : public BaseService
{
public:
	BaseClient()
	{

	}

	virtual ~BaseClient()
	{

	}

	bool add_conn(const char *host)
	{
		IOComponent *conn = connect(host, triones::TPROTOCOL_TEXT, true);
		if (NULL == conn)
		{
			return false;
		}

		_conn_list.push_back(conn);
		return true;
	}

	void some_send()
	{
		std::vector<IOComponent *>::iterator it;
		for (it = _conn_list.begin(); it != _conn_list.end(); ++it)
		{
			Packet *pack = new Packet;
			if (!pack) {
				usleep(10);
				continue;
			}

			pack->writeBytes(_send_buffer, sizeof(_send_buffer));
			IOComponent *conn = *it;
			printf("conn->_server_adapter: %p\n", conn->_server_adapter);
			printf("pack: %p\n", pack);
			if (!conn->post_packet(pack))
			{
				printf("post_packet failed\n");
				delete pack;
				pack = NULL;
			}
			else
			{
				printf("post_packet succ!\n");
			}

			// ¼õÉÙÍøÂç¶ª°üÂÊ
			usleep(10);
		}
	}

	void start(int thread)
	{
		init(thread);
	}

	virtual void handle_packet(IOComponent *ioc, Packet *packet)
	{
		if (packet->get_type() == IServerAdapter::CMD_DISCONN_PACKET)
		{
			printf("receive disconnection %s \n", ioc->get_socket()->get_peer_addr().c_str());
		}
		else if (packet->get_type() == IServerAdapter::CMD_DATA_PACKET)
		{
			printf("receive from %s len %d : %.14s \n", ioc->get_socket()->get_peer_addr().c_str(),
			        packet->getDataLen(), packet->getData());
		}
	}

private:
	std::vector<IOComponent *> _conn_list;
};

} /* namespace triones */
#endif /* BASECLIENT_H_ */
