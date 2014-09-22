#include "stdio.h"
#include "baseserver.h"
#include "testeventreactor.h"
#include "pack/tprotocol.h"
#include  "log/comlog.h"

int main(int argc, char**argv)
{
	CHGLOG("./tcpserver.log");
	SETLOGLEVEL(3);
	CHGLOGSIZE(10);
	CHGLOGNUM(20);

	bool ret = false;
	TestEventReactor *event_reactor = new TestEventReactor;
	TransProtocolText *sock_tprotocol = new TransProtocolText;

	BaseServer baseserver;
	baseserver.tcp_svr_init(event_reactor, sock_tprotocol);
	ret = baseserver.tcp_svr_start("192.168.100.48", 16000, 2);

	while (true)
	{
		sleep(1);
	}

}
