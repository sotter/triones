/******************************************************
 *   FileName: vos_timer.h
 *     Author: Triones  2012-8-15
 *Description: 移植了libevent的定时器实现，封装了统一的接口。
 *******************************************************/

#ifndef VOS_TIMER_H_
#define VOS_TIMER_H_

#define RELATIVE_TIMER 1
#define ABSOLUTE_TIMER 2

#ifndef __WINDOWS
#include <sys/time.h>
#else
#include <windows.h>
#endif

namespace vos
{

struct event;
typedef struct min_heap
{
	struct event** p;
	unsigned n, a;
} min_heap_t;

class Timer
{
public:
	Timer();
	virtual ~Timer();
    /**************************************
     * input: interval: 每次执行的时间隔间, 单位是毫秒。
     *        fun arg : 回调函数以及参数。
     *        flag    : 绝对定时器还是相对定时器，如果是相对定时器
     *        exe_num : 只有在相对定时器才有效，表示执行的次数。最少为1次
     * return: 生成定时器的ID
     **************************************/
	unsigned int timer_add(int interval, void (*fun)(void*), void *arg,  int flag = ABSOLUTE_TIMER,
			int exe_num = 0);
    /***************************************
     * description:
     * 去掉已经加入的定时器，比如产生定时器的母体已经消亡了，在消亡之间要将其删除。
     * 相对定时器在任务完成后会Timer会自己释放掉。
     ***************************************/
	bool timer_remove(unsigned int timer_id);
    /***************************************
     * description: Timer属于被动对象，没有自己的执行线程，属于被调用者。这样主要是为了避免产生线程同步。
     * 定时器的循环处理函数，由定时器的拥有者进行循环调用。它的最小时间间隔决定了定时器的精度。
     ***************************************/
	int timer_process(struct timeval *now);
//
	int timer_process();

private:
	struct min_heap _min_heap;
    unsigned int _timer_id;
};

} /* namespace vos */
#endif /* VOS_TIMER_H_ */
