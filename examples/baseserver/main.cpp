/******************************************************
 *   FileName: main.cpp
 *     Author: triones  2014-10-9
 *Description:
 *******************************************************/

#include "baseserver.h"
#include  "comm/comlog.h"

int main()
{
	CHGLOG("test.log");
	SETLOGLEVEL(3);

	BaseServer base_server;
	//base_server.start("tcp:127.0.0.1:7406", 1);
	//base_server.start("udp:127.0.0.1:7407", 1);
	base_server.start("tcp:127.0.0.1:9494", 1);

	for (;;)
	{
		sleep(1);
	}

	base_server.destroy();
	LOGSTOP();

//	IOComponent *ioc = new TCPAcceptor(NULL, NULL, NULL, NULL);
//	delete ioc;

}

