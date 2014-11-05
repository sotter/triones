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
	_serverAdapter = serverAdapter;
}

bool UDPAcceptor::init(bool isServer)
{
	UNUSED(isServer);
	_socket->setSoBlocking(false);

	return _socket->udpBind();
}

bool UDPAcceptor::handleReadEvent()
{
	return readData();
}

bool UDPAcceptor::readData()
{
	struct sockaddr_in read_addr;
	int n = _socket->recvfrom(_read_buff, sizeof(_read_buff), read_addr);
	if (n < 0) return false;

	uint64_t sockid = triones::sockutil::sock_addr2id(&read_addr);
	UDPComponent *ioc = get(sockid);
	_lastUseTime = triones::CTimeUtil::getTime();

	int decode = _streamer->decode(_read_buff, n, &_inputQueue);

	if (decode > 0)
	{
		Packet *pack = NULL;
		while ((pack = _inputQueue.pop()) != NULL)
		{
			_serverAdapter->synHandlePacket(ioc, pack);
		}
	}

	return true;
}

bool UDPAcceptor::writeData()
{
	return true;
}

void UDPAcceptor::checkTimeout(int64_t now)
{
	UNUSED(now);
	return;
}

//根据sockid获取对应的UDPComponent, 如果没有找到新建一个
UDPComponent *UDPAcceptor::get(uint64_t sockid)
{
	UDPComponent *ioc = NULL;

	_mutex.lock();
	std::map<uint64_t, UDPComponent*>::iterator iter = _mpsock.find(sockid);
	if (iter != _mpsock.end())
	{
		ioc = iter->second;
		_mutex.unlock();
	}

	ioc = new UDPComponent(NULL, _socket, _streamer, _serverAdapter, TRIONES_UDPACTCONN);
	//设置新建立的UDPComponent的地址；
	sockutil::sock_id2addr(sockid, &(ioc->_sock_addr));

	_online.push(ioc);
	_mpsock.insert(make_pair(sockid, ioc));

	_mutex.unlock();

	return ioc;
}

//将回收的ioc放回到池子中
void UDPAcceptor::put(UDPComponent* ioc)
{
	//todo:暂不实现
	UNUSED(ioc);
	return;
}

} /* namespace triones */
