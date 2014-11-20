/******************************************************
 *   FileName: UDPAcceptor.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"

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
	_socket->set_so_blocking(false);

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
	int n = _socket->recvfrom(_read_buff, sizeof(_read_buff), read_addr);
	if (n < 0) return false;

	//注意sockdi的获取方式
	uint64_t sockid = triones::sockutil::sock_addr2id(&read_addr, true);
	UDPComponent *ioc = get(sockid);
	_last_use_time = triones::CTimeUtil::get_time();

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
UDPComponent *UDPAcceptor::get(uint64_t sockid)
{
	UDPComponent *ioc = NULL;

	ioc = (UDPComponent*)(get_owner()->_hash_socks->get(sockid));

	if(ioc == NULL)
	{
		ioc = new UDPComponent(NULL, _socket, _streamer, _server_adapter, TRIONES_UDPACTCONN);
		ioc->setid(sockid);
		if(!ioc->init())
		{
			delete ioc;
			return NULL;
		}
		get_owner()->_hash_socks->put(ioc);
	}

	return ioc;
}

} /* namespace triones */
