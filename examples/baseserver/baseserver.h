/******************************************************
 *   FileName: baseserver.h
 *     Author: triones  2014-10-8
 *Description:
 *******************************************************/

#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "net/cnet.h"
#include "pack/tprotocol.h"

namespace triones
{

class BaseServer : public BaseService
{
public:

	BaseServer();
	virtual ~BaseServer();

	/*
	 * @param host: 格式"tcp:127.0.0.1:7406"或"tcp:127.0.0.1:7406"
	 * @param thread: 业务处理线程个数
	 */
	void start(const char *host, int thread = 1)
	{
		init(thread);
		IOComponent* ioc = this->listen(host, triones::TPROTOCOL_TEXT);
		if(ioc == NULL)
		{
			printf("listen error \n");
		}

		return;
	}

	virtual void handle_packet(IOComponent *ioc, Packet *packet)
	{
//		printf("receive from %s len %d : %s \n",
//				ioc->getSocket()->getAddr().c_str(),
//				packet->getDataLen(),
//				packet->getData());

		// 将收到的数据发送出去，类似于回显服务
		Packet *pack = new Packet;
		pack->writeBytes(_send_buffer, sizeof(_send_buffer));
		if (!ioc->post_packet(pack))
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
