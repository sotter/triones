#include "stdio.h"
#include "baseclient.h"
#include "testeventreactor.h"
#include "tprotocol.h"
#include "tcpsocket.h"
#include "udpsocket.h"
#include  "comlog.h"
#include "heart.h"

int main(int argc, char**argv)
{
	CHGLOG("./client.log");
	SETLOGLEVEL(3);
	CHGLOGSIZE(10);
	CHGLOGNUM(20);

	ThreadManager mgr;
	Heart* client_heart = new Heart;
	TestEventReactor *event_reactor = new TestEventReactor;
	TransProtocolText *sock_tprotocol = new TransProtocolText;

	BaseClient baseclient;
	TcpSocket* tcpcon = NULL;
	baseclient.tcp_client_start(event_reactor, sock_tprotocol);
	tcpcon = baseclient.tcp_cl_connect("192.168.100.48", 16000, 1);
	if (tcpcon != NULL)
	{
		mgr.init(1, tcpcon, client_heart);
		mgr.start();
	}

	while (true)
	{
		sleep(5);
	}
}
