#ifndef _TRIONES_HEART_H
#define _TRIONES_HEART_H
#include "thread.h"
#include "socket.h"

class Heart: public Runnable
{
public:
	virtual void run(void *param)
	{
		char buf[50] = "client heart";
		while (true)
		{
			((Socket*) param)->send(buf, strlen(buf));
			sleep(5);
		}
	}
};

#endif
