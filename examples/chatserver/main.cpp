/*
 * main.cpp
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */
#include <stdio.h>
#include "comm/comlog.h"
#include "chatserver.h"

int main(int argc, char *argv[])
{
	CHGLOG("test.log");
	SETLOGLEVEL(3);

	ChatServer chatserver;
	chatserver.start("tcp:127.0.0.1:9494", 1);

	printf("chatserver is running...\n");
	for (;;)
	{
		sleep(5);
	}

	chatserver.destroy();
	LOGSTOP();

	return 0;
}


