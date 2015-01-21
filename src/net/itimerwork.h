/*
 * itimerwork.h
 *
 *  Created on: 2014年12月29日
 *      Author: water
 */

#ifndef ITIMERWORK_H_
#define ITIMERWORK_H_

namespace triones
{

class ITimerWork
{
public:
	virtual ~ITimerWork()
	{
	}

	virtual void handle_timer_work(uint64_t now) = 0;
};

} /* namespace triones */

#endif /* ITIMERWORK_H_ */
