/******************************************************
*   FileName: main.cpp
*     Author: triones  2014-10-9
*Description:
*******************************************************/

#include "baseclient.h"
#include "comm/comlog.h"
#include "net/hashioc.h"


int main()
{
	CHGLOG("test.log");
	SETLOGLEVEL(3);

//	triones::Socket *socket = new Socket;
//
//	socket->connect("127.0.0.1", 7406, Socket::TRIONES_SOCK_TCP);
//
//	socket->show_addr();
//
//	socket->setup(socket->get_fd());
//
//	TCPComponent *ioc = new TCPComponent(NULL, socket, NULL, NULL,
//	        IOComponent::TRIONES_TCPACTCONN);
//
//	if(!ioc->init())
//	{
//		printf("component init fail \n");
//	}
//
//	Transport tp;
//	HashSock *hash_sock = new HashSock(&tp);
//	hash_sock->put(ioc);
//	hash_sock->remove(ioc);
////	hash_sock->check_timeout();
//	hash_sock->distroy();
//
//	delete hash_sock;

	BaseClient base_client;
	//BaseServer base_server;
//	base_server.start("tcp:127.0.0.1:7806", 1);
//	base_client.start("tcp:127.0.0.1:7806", 1);

	//base_client.add_conn("udp:127.0.0.1:5438");
	base_client.start(1);
	for (int i = 0; i < 100; ++i)
	{
		base_client.add_conn("udp:127.0.0.1:5438");
	}

	uint64_t count = 0;
	for (int i = 0; i < 10; i++)
	{
		base_client.some_send();
		printf("some_send count: %lu\n", ++count);
		sleep(1);
	}

	base_client.destroy();
	//base_server.destroy();

	OUT_INFO(NULL, 0, NULL, "good luck");
//	sleep(4);

	LOGSTOP();
}


