/******************************************************
 *   FileName: UDPComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "../pack/tprotocol.h"
#include "../comm/comlog.h"
#include "tbtimeutil.h"
#include "stats.h"

namespace triones
{
UDPComponent::UDPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter, int type)
		: IOComponent(owner, socket, type)
{
	_start_conn_time = 0;
	_streamer = streamer;
	_server_adapter = serverAdapter;
}

UDPComponent::~UDPComponent()
{
	if (get_type() == TRIONES_UDPCONN)
	{
		if (_socket)
		{
			_socket->close();
			delete _socket;
			_socket = NULL;
		}
	}
}

bool UDPComponent::init()
{
	int type = get_type();
	if (type == IOComponent::TRIONES_UDPCONN)
	{
		// 注意udp采用阻塞套接字，但只是写阻塞，读非阻塞
		// _socket->set_so_blocking(false);

		if (!socket_connect())
		{
			return false;
		}

		if(!_socket->setup(_socket->get_fd(), NULL, NULL, false))
		{
			return false;
		}

		this->setid(_socket->get_sockid(false));
	} else if (type == IOComponent::TRIONES_UDPACTCONN) {
		set_state(TRIONES_CONNECTED);
	}

	//对于TRIONES_UDPACTCONN类型不做任何操作，它的sockid是在外部获取的
	return true;
}

bool UDPComponent::socket_connect()
{
	// 注意，这个函数必须是可重入的，可能有不同用户的线程调用这个接口，完全依靠state判断也不是很严密的；2014-10-11
	if (get_state() == TRIONES_CONNECTED || get_state() == TRIONES_CONNECTING)
	{
		return true;
	}

	// UDP用阻塞套接字
	// _socket->set_so_blocking(false);

	// 设置连接时间
	_start_conn_time = triones::CTimeUtil::get_time();

	// 发起连接
	if (!_socket->connect())
	{
		int error = Socket::get_last_error();
		_socket->close();
		set_state(TRIONES_CLOSED);
		OUT_ERROR(NULL, 0, NULL, "connect %s fail, %s(%d)", _socket->get_peer_addr().c_str(),
			strerror(error), error);
		return false;
	}

	// 为了统一起见,这里不关心连接是成功还是EINPROGRESS或EWOULDBLOCK，都注册事件，让事件处理线程来统一地处理连接成功或失败操作
	set_state(TRIONES_CONNECTING);
	if (_sock_event)
	{
		_sock_event->add_event(_socket, true, true);
	}
	return true;
}

void UDPComponent::close()
{
	if(get_state() == TRIONES_CLOSED)
		return;

	//对于TRIONES_UDPCONN要将socket从epoll中删掉，同时将socket关闭
	if (get_type() == TRIONES_UDPCONN && _sock_event)
	{
		_sock_event->remove_event(_socket);
	}

	//只有 TRIONES_CONNECTED 和 TRIONES_CONNECTING会有回调
	if (is_conn_state())
	{
		set_state(TRIONES_CLOSED);
		//业务层的回调这个回调时需要排队的
		if(_server_adapter != NULL)
		{
			_server_adapter->syn_handle_packet(this, new Packet(IServerAdapter::CMD_DISCONN_PACKET));
		}
		OUT_ERROR(NULL, 0, NULL, "class %s", __PRETTY_FUNCTION__);
	}

	if (get_type() == TRIONES_UDPCONN)
	{
		//将socket真正的关闭
		_socket->close();
	}
}

//UDPComponent不处理可写事件，UDPComponent的写操作是同步接口
bool UDPComponent::handle_write_event()
{
	// UDP只设置可写一次，目的就是改变其连接状态
	this->enable_write(false);

	// 设置连接状态
	if (get_state() == TRIONES_CONNECTING)
	{
		int error = _socket->get_soerror();
		bool is_connected = false;

		if (error == 0)
		{
			set_state(TRIONES_CONNECTED);
			is_connected = true;
			//printf("connect %s success \n", _socket->get_peer_addr().c_str());
			OUT_INFO(NULL, 0, NULL, "connect %s success", _socket->get_peer_addr().c_str());
		}
		else
		{
			if (_sock_event)
			{
				_sock_event->remove_event(_socket);
			}
			_socket->close();
			set_state(TRIONES_CLOSED);

			OUT_ERROR(NULL, 0, NULL, "connect %s fail: %s(%d)", _socket->get_peer_addr().c_str(),
				strerror(error), error);
			//printf("connect %s fail: %s(%d) \n", _socket->get_peer_addr().c_str(),
				//strerror(error), error);
		}

		// 调用回调函数
		_server_adapter->handle_connected(this, is_connected);
	}

	// 返回成功
	return true;
}

//UDPComponent只有TRIONES_UDPCONN才处理写事件
bool UDPComponent::handle_read_event()
{
	__INTO_FUN__
	_last_use_time = triones::CTimeUtil::get_time();
	bool rc = false;
	if (get_type() == TRIONES_UDPCONN)
	{
		rc = read_data();
	}
	return rc;
}

//提供给已经连接的UDPSocket使用；
bool UDPComponent::read_data()
{
	__INTO_FUN__

	//只有TRIONES_UDPCONN类型的才可以调用到这个地方
	if (get_type() != TRIONES_UDPCONN)
	{
		OUT_INFO(NULL, 0, NULL, "read type error, type %d", _type);
		return false;
	}

	int n = _socket->nonblock_read(_read_buff, sizeof(_read_buff));
	if (n < 0) {
		int error = Socket::get_last_error();
		bool result = error == EAGAIN ? true : false;
		return result;
	}

	int decode = _streamer->decode(_read_buff, n, &_inputQueue);

	if (decode > 0)
	{
		Packet *pack = NULL;
		while ((pack = _inputQueue.pop()) != NULL)
		{
			_server_adapter->syn_handle_packet(this, pack);
		}
	}

	return true;
}

//提供给已经连接的UDPSocket使用；
bool UDPComponent::write_data()
{
	__INTO_FUN__
	return true;
}

//数据发送的同步接口
bool UDPComponent::post_packet(Packet *packet)
{
	//TRIONES_UDPCONN和TRIONES_UDPACTCONN可以调用到这个地方
	if (get_type() != TRIONES_UDPCONN && get_type() != TRIONES_UDPACTCONN)
	{
		OUT_INFO(NULL, 0, NULL, "write type error, type %d", get_type());
		return false;
	}

	// 处于未连接状态，发送失败
	if (!is_conn_state())
	{
		return false;
	}

	int len = packet->getDataLen();
	int offset = 0;
	int sendbytes = 0;
	int send_len = 0;

	bool ret = true;
	while (offset < len)
	{
		//UDP包的最大发送长度TRIONES_UDP_MAX_PACK
		send_len = min(len, TRIONES_UDP_MAX_PACK);

		//如果是已经调用connect的情况
		if (get_type() == TRIONES_UDPCONN)
		{
			sendbytes = _socket->write(packet->getData() + offset, send_len);
		}
		else if (get_type() == TRIONES_UDPACTCONN)
		{
			sendbytes = _socket->sendto(packet->getData() + offset, send_len, _sock_addr);
		}

		if (sendbytes < 0)
		{
			ret = false;
			break;
		}

		offset += sendbytes;
	}

	if (ret) delete packet;

	return ret;
}

//todo : 2014-10-29
bool UDPComponent::check_timeout(uint64_t now)
{
	//printf("into UDPComponent::checkTimeout \n");

	//client connect的超时时间
	const uint64_t conn_timeout = (2 * 1000 * 1000);
	//重连时间间隔
	const uint64_t reconn_time = (10 * 1000 * 1000);
	//普通socket没有数据时的超时时间
	const uint64_t timeout = static_cast<uint64_t>(18 * 1000 * 1000);

	int type = get_type();		// 获取UDPComponent的类型,是客户端主动发起的还是服务端派生出来的

	bool ret = false;
	if (get_state() == TRIONES_CONNECTING)
	{
		if (_start_conn_time > 0 && _start_conn_time < (now - conn_timeout))
		{
			// 连接超时 2 秒
			set_state(TRIONES_CLOSED);
			OUT_ERROR(NULL, 0, NULL, "connect to %s timeout", _socket->get_addr().c_str());
			// 关闭写端
			_socket->shutdown();
			ret = true;
		}
	}
	else if (get_state() == TRIONES_CONNECTED)
	{
		//客户端不主动做超时检测，只要服务端不给断就一直连接着
		if (type == TRIONES_UDPACTCONN)
		{
			// 连接的时候, 只用在服务器端
			uint64_t last_use_time = _last_use_time;
			if (last_use_time < now - timeout)
			{
				uint64_t idle = now - last_use_time;
				OUT_INFO(NULL, 0, NULL, "%s %d(s) with no data, disconnect it",
					_socket->get_peer_addr().c_str(), (idle / static_cast<uint64_t>(1000000)));

				// 不要shutdown,因为所有派生出的udp之ioc，都共用一个_socket
				// _socket->shutdown();
				// set_state(TRIONES_CLOSED);
				ret = true;
			}
		}
	}
	else if (get_state() == TRIONES_CLOSED)
	{
		if (type == TRIONES_UDPCONN && _auto_reconn)
		{
			// 每隔五秒钟重连一次
			if (_start_conn_time > 0 && _start_conn_time < (now - reconn_time))
			{
				// 不管是否连接成功，都更新连接时间，时间间隔都是5000000
				_start_conn_time = triones::CTimeUtil::get_time();
				socket_connect();
			}
		}
	}

	// true代表超时，false代表未超时
	return ret;
}

//std::string UDPComponent::info()
//{
//
//}


} /* namespace triones */
