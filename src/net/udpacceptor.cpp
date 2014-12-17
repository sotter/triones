/******************************************************
 *   FileName: UDPAcceptor.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "../comm/comlog.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace triones
{

UDPAcceptor::UDPAcceptor(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter)
		: IOComponent(owner, socket, TRIONES_UDPACCETOR)
{
	_streamer = streamer;
	_server_adapter = serverAdapter;
}

bool UDPAcceptor::init()
{
	// 注意udp采用阻塞套接字，但只是写阻塞，读非阻塞
	// _socket->set_so_blocking(false);

	if(_socket->setup(_socket->get_fd()))
	{
		this->setid(_socket->get_sockid());
		return true;
	}

	return false;
}

//IOComponent 网络资源清理
void UDPAcceptor::close()
{
	_socket->close();
}

bool UDPAcceptor::handle_read_event()
{
	return read_data();
}

bool UDPAcceptor::read_data()
{
	struct sockaddr_in read_addr;
	int n = _socket->nonblock_recvfrom(_read_buff, sizeof(_read_buff), read_addr);
	if (n < 0) return false;

	//注意sockdi的获取方式
	uint64_t sockid = triones::sockutil::sock_addr2id(&read_addr, true);
	UDPComponent *ioc = get(sockid, &read_addr);
	if (NULL == ioc)
	{
		// 返回true,否则Transport::event_loop会对UDPAcceptor执行remove_component操作
		return true;
	}

	// 设置IOC最后时间
	uint64_t last_use_time = triones::CTimeUtil::get_time();
	ioc->set_last_use_time(last_use_time);
	_last_use_time = last_use_time;


	int decode = _streamer->decode(_read_buff, n, &_inputQueue);

	if (decode > 0)
	{
		Packet *pack = NULL;
		while ((pack = _inputQueue.pop()) != NULL)
		{
			_server_adapter->syn_handle_packet(ioc, pack);
		}
	}

	return true;
}

bool UDPAcceptor::write_data()
{
	return true;
}

bool UDPAcceptor::check_timeout(uint64_t now)
{
	UNUSED(now);
	return false;
}

//根据sockid获取对应的UDPComponent, 如果没有找到新建一个
UDPComponent *UDPAcceptor::get(uint64_t sockid, struct sockaddr_in *addr)
{
	UDPComponent *ioc = NULL;

	ioc = (UDPComponent*)(get_owner()->_hash_socks->get(sockid));

	if(ioc == NULL)
	{
		OUT_INFO(NULL, 0, "UDPAcceptor", "new an UDPComponent %"PRIu64"", sockid);
		ioc = new UDPComponent(NULL, _socket, _streamer, _server_adapter, TRIONES_UDPACTCONN);
		ioc->setid(sockid);
		ioc->set_addr(addr);
		if(!ioc->init())
		{
			delete ioc;
			return NULL;
		}

		if (get_owner()->_hash_socks->put(ioc))
		{
			ioc->set_used(true);
			ioc->add_ref();
		}
	}

	return ioc;
}

} /* namespace triones */
