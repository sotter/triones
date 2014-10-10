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

	void init();

	void start(const char *host, int thread)
	{
		_transport->start();
		_transport->connect(host, _tp, true);
	}

	virtual void handle_queue_packet(IOComponent *ioc, Packet *packet)
	{
		printf("receive from %s len %d : %s \n",
				ioc->getSocket()->getAddr().c_str(),
				packet->getDataLen(),
				packet->getData());
	}

private:

	TransProtocol *_tp;
};

} /* namespace triones */
#endif /* BASECLIENT_H_ */
