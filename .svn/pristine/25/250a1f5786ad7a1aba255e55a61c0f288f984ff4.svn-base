/******************************************************
 *   FileName: vos_sem.h
 *     Author: Triones  2012-7-6 
 *Description:
 *******************************************************/

#ifndef VOS_SEM_H_
#define VOS_SEM_H_

#ifdef __WINDOWS
#include <windows.h>
#else
#include <semaphore.h>
#endif

namespace vos
{
class Sem
{
public:
	//第二个参数window下使用
	Sem(unsigned int value = 0, unsigned int max_value = 0);
	~Sem();

public:
	int init(unsigned int value, unsigned int max_value = 1024 * 1024);
	int uninit();

	int sem_wait();
	int sem_trywait();
	int sem_timewait(unsigned int time_out);
	int sem_post();
	int sem_getvalue();

private:

#ifndef __WINDOWS
	sem_t		_sem;
#else
	HANDLE _sem;
#endif

	bool		_inited;
	unsigned int	_value;

};

} /* namespace vos */
#endif /* VOS_SEM_H_ */
