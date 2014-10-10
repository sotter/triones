/******************************************************
*   FileName: main.cpp
*     Author: triones  2014-10-9
*Description:
*******************************************************/

#include "baseserver.h"

int main()
{
	BaseServer base_server;
	base_server.start("tcp:127.0.0.1:7406", 1);

	while(1)
	{
		sleep(100);
	}
}

