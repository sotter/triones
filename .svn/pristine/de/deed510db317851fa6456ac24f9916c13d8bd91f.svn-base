#include "stdio.h"
#include "baseclient.h"
#include "net/testeventreactor.h"
#include "pack/tprotocol.h"
#include "net/tcpsocket.h"
#include  "comlog.h"

int main(int argc, char**argv)
{
	CHGLOG("./client.log");
	SETLOGLEVEL(3);
	CHGLOGSIZE(10);
	CHGLOGNUM(20);

	UdpSocket* _udp_sock = NULL;
	TcpSocket  *tcp_socket = NULL;
	TestEventReactor *event_reactor = new TestEventReactor;
	TransProtocolText *sock_tprotocol = new TransProtocolText;


	BaseClient baseclient;
	baseclient.udp_client_start(event_reactor, sock_tprotocol);
	tcp_socket = baseclient.tcp_cl_connect("192.168.100.48", 16000, 8999);

	if (NULL != tcp_socket && tcp_socket->_state == ESTABLISHED)
	{
		char send_buf[100] = "[wangshitao---pingpang-udp]";
		_udp_sock->send(send_buf, sizeof(send_buf));
		OUT_INFO(NULL, 0, NULL, " start client success");
	}

	UdpSocket *_udp_sock2 = NULL;
	_udp_sock2 = baseclient.udp_cl_connect("192.168.100.48", 16000, 9000, 1);

//	BaseClient baseclient;
//	baseclient.udp_client_start(event_reactor, sock_tprotocol);
//	_udp_sock = baseclient.udp_cl_connect("192.168.100.48", 16000, 8999, 1);
//	if (NULL != _udp_sock && _udp_sock->_state == ESTABLISHED)
//	{
//		char send_buf[100] = "[wangshitao---pingpang-udp]";
//		_udp_sock->send(send_buf, sizeof(send_buf));
//		OUT_INFO(NULL, 0, NULL, " start client success");
//	}
//
//	UdpSocket *_udp_sock2 = NULL;
//	_udp_sock2 = baseclient.udp_cl_connect("192.168.100.48", 16000, 9000, 1);

	while (true)
	{
		sleep(5);
	}
}
