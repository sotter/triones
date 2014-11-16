/******************************************************
*   FileName: main.cpp
*     Author: triones  2014-10-9
*Description:
*******************************************************/

#include "baseclient.h"
#include "comm/comlog.h"


int main()
{
	CHGLOG("test.log");

	triones::Socket *socket = new Socket;

	socket->connect("127.0.0.1", 7406, Socket::TRIONES_SOCK_TCP);

	socket->show_addr();

	socket->connect("127.0.0.1", 7406, Socket::TRIONES_SOCK_TCP);

	socket->show_addr();

//	BaseClient base_client;
////	BaseServer base_server;
////	base_server.start("tcp:127.0.0.1:7406", 1);
////	sleep(1);
//	base_client.start("tcp:127.0.0.1:7406", 1);
//
////	base_client.start("udp:127.0.0.1:7407", 1);
////	base_server.start("udp:127.0.0.1:7407", 1);
//
//	while (1)
//	{
//		sleep(3);
//	}
//
//	base_client.destroy();
////	base_server.destroy();
//
//	OUT_INFO(NULL, 0, NULL, "good luck");
//	sleep(4);

	LOGSTOP();

}


