
#include "baseclient.h"

BaseClient::BaseClient()
{
	_tcp = new TcpTransPort();
	_udp = new UdpTransPort();
}

BaseClient::~BaseClient()
{
	if (NULL != _tcp)
	{
		tcp_cl_stop();
		delete _tcp;
		_tcp = NULL;

	}

	if (NULL != _udp)
	{
		udp_cl_stop();
		delete _udp;
		_udp = NULL;
	}
}

bool BaseClient::tcp_client_start(IEventReactor *reactor, TransProtocol* protocol)
{
	return _tcp->start(reactor, protocol);
}

TcpSocket* BaseClient::tcp_cl_connect(char*peer_ip, int peer_port, int thread_num)
{
	return _tcp->tcp_connect(peer_ip, peer_port, thread_num);
}

bool BaseClient::udp_client_start(IEventReactor *reactor, TransProtocol* protocol)
{
	return _udp->start(reactor, protocol);
}

UdpSocket* BaseClient::udp_cl_connect(char*peer_ip, int peer_port, int local_port, int thread_num)
{
	return _udp->udp_cl_connect(peer_ip, peer_port, local_port, thread_num);
}

int BaseClient::tcp_send(Socket* sock, const char* data_buf, int data_len)
{
	return _tcp->send_msg(sock, data_buf, data_len);
}

int BaseClient::udp_send(Socket* sock, const char* data_buf, int data_len)
{
	return _udp->send_msg(sock, data_buf, data_len);
}

bool BaseClient::tcp_cl_stop()
{
	return _tcp->stop();
}

bool BaseClient::udp_cl_stop()
{
	return _udp->stop();
}

