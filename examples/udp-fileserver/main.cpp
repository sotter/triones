/******************************************************
*  CopyRight: 北京中交兴路科技有限公司(2012-2015)
*   FileName: main.cpp
*     Author: Triones  2012-10-15 
*Description:
*******************************************************/

#include "fileserver.h"
#include <unistd.h>
#include "log.h"


int main()
{
	INIT_LOG(NULL, LOG_CONSOLE, "test.log", 1);
	FileServer file_server;
	file_server.start("0.0.0.0", 8808, new FileService);



//    struct sockaddr_in addr;
//
//    addr.sin_addr.s_addr = inet_addr("192.168.5.214");
//    addr.sin_port = htons(36550);
//
//    printf("index = %lld \n", get_index(&addr));
//
//    addr.sin_port = htons(56485);
//    printf("index = %lld \n", get_index(&addr));

	while (1)
	{
		sleep(5);
	}

    return 0;
}

