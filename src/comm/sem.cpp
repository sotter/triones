/**
 * author: Triones
 * date  : 2012-07-09
 * */
#include <stddef.h>
#include <semaphore.h>
#include <limits.h>
#include "sem.h"

namespace triones
{

Sem::Sem()
		: _sem(NULL)
{
	_sem = (void*) new sem_t;
	init();
}

Sem::~Sem()
{
	destroy();
	delete ((sem_t*) _sem);
	_sem = NULL;
}

void Sem::init(unsigned int value /*= 0*/)
{
	if (value > SEM_VALUE_MAX)
	{
		value = SEM_VALUE_MAX;
	}

	sem_init((sem_t*) _sem, 0, value);
}

void Sem::wait()
{
	while (1)
	{
		if (0 == sem_wait((sem_t*) _sem))
		{
			return;
		}
	}
}

#ifdef __USE_XOPEN2K
bool Sem::wait(time_t timeout)
{
	if (timeout == 0)
	{
		wait();
		return true;
	}

	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	timeout = timeout - ts.tv_sec * 1000;
	ts.tv_nsec = timeout * 1000000;

	if (0 == sem_timedwait((sem_t*) _sem, &ts))
	{
		return true;
	}

	return false;

}
#endif

bool Sem::trywait()
{
	if (0 == sem_trywait((sem_t*) _sem))
	{
		return true;
	}

	return false;
}

void Sem::post()
{
	sem_post((sem_t*) _sem);
}

void Sem::destroy()
{
	sem_destroy((sem_t*) _sem);
}

} // namespace triones

