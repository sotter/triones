
#include "baseserver.h"

BaseServer::BaseServer()
{
	_tcp = new TcpTransPort();
	_udp = new UdpTransPort();
}

BaseServer::~BaseServer()
{
	if (NULL != _tcp)
	{
		tcp_svr_stop();
		delete _tcp;
		_tcp = NULL;
	}

	if (NULL != _udp)
	{
		udp_svr_stop();
		delete _udp;
		_udp = NULL;
	}
}

bool BaseServer::tcp_svr_init(IEventReactor *reactor, TransProtocol* protocol)
{
	return _tcp->start(reactor, protocol);
}

bool BaseServer::udp_svr_init(IEventReactor *reactor, TransProtocol* protocol)
{
	return _udp->start(reactor, protocol);
}

bool BaseServer::tcp_svr_start(const char *ip, unsigned short port, int thread_num)
{
	return _tcp->tcp_svr_listen(ip, port, thread_num);
}

bool BaseServer::udp_svr_start(const char *ip, unsigned short port, int thread_num)
{
	return _udp->udp_svr_startup(ip, port, thread_num);
}

bool BaseServer::tcp_svr_stop()
{
	return _tcp->stop();
}

bool BaseServer::udp_svr_stop()
{
	return _udp->stop();
}

int BaseServer::tcp_send(Socket* sock, const char* data_buf, int data_len)
{
	return _tcp->send_msg(sock, data_buf, data_len);
}

int BaseServer::udp_send(Socket* sock, const char* data_buf, int data_len)
{
	return _udp->send_msg(sock, data_buf, data_len);
}

