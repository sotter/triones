/******************************************************
*   FileName: main.cpp
*     Author: triones  2014-10-9
*Description:
*******************************************************/

#include "baseclient.h"

int main()
{
	BaseClient base_client;
	base_client.start("tcp:127.0.0.1:7406", 1);

	sleep(5);
	base_client.destroy();

}

