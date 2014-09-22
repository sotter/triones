/******************************************************
*  CopyRight: 北京中交兴路科技有限公司(2012-2015)
*   FileName: test.cpp
*     Author: Triones  2012-9-18 
*Description:
*******************************************************/


#include "fileclient.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include "log.h"

//int i = EPOLL_CTL_ADD;

#define defaultip   "127.0.0.1"
#define defaultport "8808"

void *thread_fun(void *arg)
{
	int a = 0;
    if(arg != NULL)
    	a = 10000;

	int num = 1000;
	char buffer[32] = {0};
	time_t btime = time(0);

    eventpool eventpool;
    eventpool.init(1);

	FileClient fc(eventpool);

	if(! fc.start("192.168.5.214", 8808))
		return false;

	while(num --)
	{
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "./test%d.jpeg", num + a);
        usleep(100);
	    fc.put_file("test.jpeg", buffer);
	}

	printf("-----------------use time %d \n", (int)(time(0) - btime));

    sleep(10);

    return NULL;
}

int main(int argc, char *argv[])
{
	INIT_LOG(NULL, LOG_CONSOLE, "test.log", 1);

	const char *ip = NULL;
	const char *port = NULL;
    if(argc < 3)
    {
        ip = defaultip;
        port = defaultport;
    }
    else
    {
    	ip = argv[1];
    	port = argv[2];
    }

    //    vos::make_thread(thread_fun, NULL);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_fun, 0);

//  pthread_create(&t2, NULL, thread_fun, (void*)10000);

	while(1)
	{
		sleep(1);
	}

    return 0;
}
