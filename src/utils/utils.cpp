/**
 * author: Triones
 * date  : 2014-08-26
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "utils.h"

namespace triones
{

void enable_coredump(bool enable)
{
	struct rlimit limit;

	if (enable)
	{
		limit.rlim_cur = RLIM_INFINITY;
		limit.rlim_max = RLIM_INFINITY;
	}
	else
	{
		limit.rlim_cur = 0;
		limit.rlim_max = 0;
	}

	setrlimit(RLIMIT_CORE, &limit);
}

#define STACKSIZE (258 * 1024 * 1024)

void greedy_for_resources()
{
	struct rlimit limit;

	// maximize the number of open files.
	if (getrlimit(RLIMIT_NOFILE, &limit) < 0)
	{
		limit.rlim_cur = 1024;
		limit.rlim_max = 1024;
	}

	while (setrlimit(RLIMIT_NOFILE, &limit) == 0)
	{
		limit.rlim_cur++;
		limit.rlim_max++;

		if (limit.rlim_cur >= RLIM_INFINITY ) break;
	}

	// try to set the maximum size of stack segment
	if (getrlimit(RLIMIT_STACK, &limit) < 0)
	{
		limit.rlim_cur = 0;
	}

	if (limit.rlim_cur < STACKSIZE)
	{
		limit.rlim_cur = STACKSIZE;
		limit.rlim_max = STACKSIZE;
		setrlimit(RLIMIT_STACK, &limit);
	}
}

} // namespace triones
