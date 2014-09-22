
#ifndef _TRIONES_BASE_CLIENT_H
#define _TRIONES_BASE_CLIENT_H
#include "tcptransport.h"
#include "udptransport.h"

class BaseClient
{
public:

	BaseClient();

	~BaseClient();

	bool tcp_client_start(IEventReactor *reactor, TransProtocol* protocol);

	TcpSocket* tcp_cl_connect(char*peer_ip, int peer_port, int thread_num = 1);

	bool udp_client_start(IEventReactor *reactor, TransProtocol* protocol);

	UdpSocket* udp_cl_connect(char*peer_ip, int peer_port, int local_port, int thread_num = 1);

	int tcp_send(Socket* sock, const char* data_buf, int data_len);

	int udp_send(Socket* sock, const char* data_buf, int data_len);

	bool tcp_cl_stop();

	bool udp_cl_stop();
private:
	UdpTransPort* _udp;
	TcpTransPort* _tcp;
};
#endif /*  */
