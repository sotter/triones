/******************************************************
 *   FileName: TCPAcceptor.cpp
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
TCPAcceptor::TCPAcceptor(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter)
		: IOComponent(owner, socket)
{
	_streamer = streamer;
	_server_adapter = serverAdapter;
}

TCPAcceptor::~TCPAcceptor()
{
		if (_socket)
		{
			_socket->close();
			delete _socket;
			_socket = NULL;
		}
}

// 初始化, 开始监听
bool TCPAcceptor::init(bool isServer)
{
	UNUSED(isServer);
	_socket->set_so_blocking(false);
	return ((ServerSocket*) _socket)->listen();
}

// 当有数据可读时被Transport调用
bool TCPAcceptor::handle_read_event()
{
	Socket *socket;
	while ((socket = ((ServerSocket*) _socket)->accept()) != NULL)
	{
		// TCPComponent, 在服务器端
		TCPComponent *component = new TCPComponent(_owner, socket, _streamer, _server_adapter);
		if (!component->init(true))
		{
			delete component;
			return true;
		}
		// 对于服务端来说，component的ID，由对端地址生成；
		component->setid(socket->get_peer_sockid());
		// 加入到iocomponents中，及注册可读到socketevent中
		_owner->add_component(component, true, false);
	}

	return true;
}

//IOComponent 网络资源清理
void TCPAcceptor::close()
{
	_socket->close();
}

// 超时检查 now 当前时间(单位us)
bool TCPAcceptor::check_timeout(uint64_t now)
{
	UNUSED(now);
	return false;
}

} /* namespace triones */
