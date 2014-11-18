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
	_streamer = streamer;
	_server_adapter = serverAdapter;
}

UDPComponent::~UDPComponent()
{
	if (_type == TRIONES_UDPCONN)
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
	if (_type == IOComponent::TRIONES_UDPCONN)
	{
		if (!_socket->connect())
		{
			return false;
		}

		if(_socket->setup(_socket->get_fd()))
		{
			this->setid(_socket->get_sockid());
			return true;
		}

		return false;
	}

	//对于TRIONES_UDPACTCONN类型不做任何操作，它的sockid是在外部获取的
	return true;
}

void UDPComponent::close()
{
	//对于TRIONES_UDPCONN要将socket从epoll中删掉，同时将socket关闭
	if (_type == TRIONES_UDPCONN && _sock_event)
	{
		_sock_event->remove_event(_socket);
	}

	//只有 TRIONES_CONNECTED 和 TRIONES_CONNECTING会有回调
	if (is_conn_state())
	{
		_state = TRIONES_CLOSED;
		//业务层的回调这个回调时需要排队的
		if(_server_adapter != NULL)
		{
			_server_adapter->syn_handle_packet(this, new Packet(IServerAdapter::CMD_DISCONN_PACKET));
		}
	}

	if (_type == TRIONES_UDPCONN)
	{
		//将socket真正的关闭
		_socket->close();
	}
}

//UDPComponent不处理可写事件，UDPComponent的写操作是同步接口
bool UDPComponent::handle_write_event()
{
	return true;
}

//UDPComponent只有TRIONES_UDPCONN才处理写事件
bool UDPComponent::handle_read_event()
{
	__INTO_FUN__
	_last_use_time = triones::CTimeUtil::get_time();
	bool rc = false;
	if (_type == TRIONES_UDPCONN)
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
	if (_type != TRIONES_UDPCONN)
	{
		OUT_INFO(NULL, 0, NULL, "read type error, type %d", _type);
		return false;
	}

	int n = _socket->read(_read_buff, sizeof(_read_buff));
	if (n < 0) return false;

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
	if (_type != TRIONES_UDPCONN && _type != TRIONES_UDPACTCONN)
	{
		OUT_INFO(NULL, 0, NULL, "write type error, type %d", _type);
		return false;
	}

	bool ret = false;
	int len = packet->getDataLen();
	int offset = 0;
	int sendbytes = 0;
	int send_len = 0;

	while (offset < len)
	{
		//UDP包的最大发送长度TRIONES_UDP_MAX_PACK
		send_len = min(len, TRIONES_UDP_MAX_PACK);

		//如果是已经调用connect的情况
		if (_type == TRIONES_UDPCONN)
		{
			sendbytes = _socket->write(packet->getData() + offset, send_len);
		}
		else if (_type == TRIONES_UDPACTCONN)
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
	printf("into UDPComponent::checkTimeout \n");

	uint64_t t = now  - static_cast<uint64_t>(10 * 1000 * 1000);

	bool ret =  (get_last_use_time() < t);

	printf("ret = %d \n", ret);

	return ret;
}


} /* namespace triones */
