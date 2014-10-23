/******************************************************
 *   FileName: UDPAcceptor.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"

namespace triones
{

/**
 * 构造函数，由Transport调用。
 *
 * @param  owner:    运输层对象
 * @param  host:   监听ip地址或hostname
 * @param port:   监听端口
 * @param streamer:   数据包的双向流，用packet创建，解包，组包。
 * @param serverAdapter:  用在服务器端，当Connection初始化及Channel创建时回调时用
 */
UDPAcceptor::UDPAcceptor(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter)
		: IOComponent(owner, socket, TRIONES_UDPACCETOR)
{
	_streamer = streamer;
	_serverAdapter = serverAdapter;
}

/*
 * 初始化, 开始监听
 */
bool UDPAcceptor::init(bool isServer)
{
	UNUSED(isServer);
	_socket->setSoBlocking(false);
	return ((ServerSocket*) _socket)->listen();
}

/**
 * 当有数据可读时被Transport调用
 *
 * @return 是否成功
 */
bool UDPAcceptor::handleReadEvent()
{

}

bool UDPAcceptor::readData()
{
	struct sockaddr_in read_addr;
	int n = _socket->recvfrom(_read_buff, sizeof(_read_buff), read_addr);

}

bool UDPAcceptor::writeData()
{

}

void UDPAcceptor::checkTimeout(int64_t now)
{
	UNUSED(now);
	return;
}

} /* namespace triones */
