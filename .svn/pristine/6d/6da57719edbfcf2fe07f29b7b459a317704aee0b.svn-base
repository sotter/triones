/**
 * author: Triones
 * date  : 2012-07-09
 * */
#ifndef __TRIONES_SEM_H__
#define __TRIONES_SEM_H__

#include <time.h>

namespace triones
{

class Sem
{
public:
	Sem();
	virtual ~Sem();

	void init(unsigned int value = 0);
	void wait();
#ifdef __USE_XOPEN2K
	bool wait(time_t timeout);
#endif
	bool trywait();
	void post();
	void destroy();

private:	// no copyable
	Sem(const Sem&);
	Sem& operator=(const Sem&);

private:
	void* _sem;
};

} // namespace triones

#endif // #ifndef __TRIONES_SEM_H__
