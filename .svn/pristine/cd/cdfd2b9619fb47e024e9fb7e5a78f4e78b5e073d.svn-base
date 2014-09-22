/******************************************************
 *   FileName: vos_timer.cpp
 *     Author: Triones  2012-8-15
 *Description:
 *******************************************************/

#include "vos_timer.h"

#ifndef __WINDOWS
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define	evutil_timercmp(tvp, uvp, cmp)							\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?							\
	((tvp)->tv_usec cmp (uvp)->tv_usec) :						\
	((tvp)->tv_sec cmp (uvp)->tv_sec))

#define	evutil_timersub(tvp, uvp, vvp)						\
	do {													\
	(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
	(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
	if ((vvp)->tv_usec < 0) {							\
	(vvp)->tv_sec--;								\
	(vvp)->tv_usec += 1000000;						\
	}													\
	} while (0)

#define evutil_timeradd(tvp, uvp, vvp)							\
	do {														\
	(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;			\
	(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
	if ((vvp)->tv_usec >= 1000000) {						\
	(vvp)->tv_sec++;									\
	(vvp)->tv_usec -= 1000000;							\
	}														\
	} while (0)

#ifdef __WINDOWS
int gettimeofday(struct timeval* tv, void * attr)
{
	union
	{
		long long ns100;
		FILETIME ft;
	}now;

	GetSystemTimeAsFileTime (&now.ft);
	tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
	tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
	return (0);
}
#endif

namespace vos
{
struct event
{
	unsigned int min_heap_idx; /* for managing timeouts */
	unsigned int timer_id;
	struct timeval ev_interval;
	struct timeval ev_timeout;
	int ev_exe_num;

	void (*ev_callback)(void *arg);
	void *ev_arg;

	int ev_res; /* result passed to event callback */
	int ev_flags;
};

/***构造函数  ***************/
static inline void min_heap_ctor(min_heap_t* s);
/***析构函数  ***************/
static inline void min_heap_dtor(min_heap_t* s);
/***初始化函数  ***************/
static inline void min_heap_elem_init(struct event* e);
/****比较函数***************/
static inline int min_heap_elem_greater(struct event *a, struct event *b);

static inline int min_heap_empty(min_heap_t* s);

static inline unsigned min_heap_size(min_heap_t* s);
/****返回栈顶元素************/
static inline struct event* min_heap_top(min_heap_t* s);
/****调整定时器的大小*********/
static inline int min_heap_reserve(min_heap_t* s, unsigned n);
/****放入数据*************/
static inline int min_heap_push(min_heap_t* s, struct event* e);
/****取得最后上面的数据******/
static inline struct event* min_heap_pop(min_heap_t* s);
/****消除一个定时器元素*******/
static inline int min_heap_erase(min_heap_t* s, struct event* e);
/****向上调整 ************/
static inline void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e);
/****向下调整************/
static inline void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e);

static inline void gettime(struct timeval *tm);

Timer::Timer() :
		_timer_id(0)
{
	min_heap_ctor(&_min_heap);
}

Timer::~Timer()
{
	for (int i = 0; i < (int)_min_heap.n; i++)
	{
		free(_min_heap.p[i]);
	}
	min_heap_dtor(&_min_heap);
}

unsigned int Timer::timer_add(int interval, void(*fun)(void*), void *arg,
		int flag /* = ABSOLUTE_TIMER */, int exe_num /* =  0 */)
{
	struct event * ev = (struct event*) malloc(sizeof(struct event));
	min_heap_elem_init(ev);
	if (NULL == ev)
		return -1;
	struct timeval now;
	gettime(&now);
	ev->ev_interval.tv_sec = interval / 1000;
	ev->ev_interval.tv_usec = (interval % 1000) * 1000;
	evutil_timeradd(&now, &(ev->ev_interval), &(ev->ev_timeout));
	ev->ev_flags = flag;
	ev->ev_callback = fun;
	ev->ev_arg = arg;
	ev->ev_exe_num = exe_num;
	ev->timer_id = _timer_id++;

	min_heap_push(&_min_heap, ev);

	return ev->timer_id;
}

bool Timer::timer_remove(unsigned int timer_id)
{
	for (int i = 0; i < (int)_min_heap.n; i++)
	{
		if (timer_id == _min_heap.p[i]->timer_id)
		{
			struct event * e = _min_heap.p[i];
			min_heap_erase(&_min_heap, _min_heap.p[i]);
            printf("timer_remove free(e) \n");
			free(e);
			return true;
		}
	}
	return false;
}

int Timer::timer_process(struct timeval *now)
{
	struct event *event;
	while ((event = min_heap_top(&_min_heap)) != NULL)
	{
		if (evutil_timercmp(now, &(event->ev_timeout), < ))
			break;
		min_heap_pop(&_min_heap);
		event->ev_callback(event->ev_arg);
		if (event->ev_flags == ABSOLUTE_TIMER || (event->ev_flags == RELATIVE_TIMER && --event->ev_exe_num > 0))
		{
			evutil_timeradd(&(event->ev_timeout), &(event->ev_interval), &(event->ev_timeout));
			min_heap_push(&_min_heap, event);
		}
		else
		{
            printf("timer_process free(event) \n");
			free(event);
		}
	}

	return 0;
}
//
int Timer::timer_process()
{
	struct timeval now;
	gettime(&now);
    return timer_process(&now);
}

void gettime(struct timeval *tm)
{
	gettimeofday(tm, NULL);
}

int min_heap_elem_greater(struct event *a, struct event *b)
{
	return evutil_timercmp(&a->ev_timeout, &b->ev_timeout, >);
}

void min_heap_ctor(min_heap_t* s)
{
	s->p = 0;
	s->n = 0;
	s->a = 0;
}

void min_heap_dtor(min_heap_t* s)
{
	if (s->p)
		free(s->p);
}

void min_heap_elem_init(struct event* e)
{
	e->min_heap_idx = -1;
}

int min_heap_empty(min_heap_t* s)
{
	return 0u == s->n;
}

unsigned min_heap_size(min_heap_t* s)
{
	return s->n;
}

struct event* min_heap_top(min_heap_t* s)
{
	return s->n ? *s->p : 0;
}

int min_heap_push(min_heap_t* s, struct event* e)
{
	if (min_heap_reserve(s, s->n + 1))
		return -1;
	min_heap_shift_up_(s, s->n++, e);
	return 0;
}

struct event* min_heap_pop(min_heap_t* s)
{
	if (s->n)
	{
		struct event* e = *s->p;
		min_heap_shift_down_(s, 0u, s->p[--s->n]);
		e->min_heap_idx = -1;
		return e;
	}
	return 0;
}

int min_heap_erase(min_heap_t* s, struct event* e)
{
	if (((unsigned int) -1) != e->min_heap_idx)
	{
		struct event *last = s->p[--s->n];
		unsigned parent = (e->min_heap_idx - 1) / 2;
		/* we replace e with the last element in the heap.  We might need to
		 shift it upward if it is less than its parent, or downward if it is
		 greater than one or both its children. Since the children are known
		 to be less than the parent, it can't need to shift both up and
		 down. */
		if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
			min_heap_shift_up_(s, e->min_heap_idx, last);
		else
			min_heap_shift_down_(s, e->min_heap_idx, last);
		e->min_heap_idx = -1;
		return 0;
	}
	return -1;
}

int min_heap_reserve(min_heap_t* s, unsigned n)
{
	if (s->a < n)
	{
		struct event** p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (struct event**) realloc(s->p, a * sizeof *p)))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}

void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e)
{
	unsigned parent = (hole_index - 1) / 2;
	while (hole_index && min_heap_elem_greater(s->p[parent], e))
	{
		(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e)
{
	unsigned min_child = 2 * (hole_index + 1);
	while (min_child <= s->n)
	{
		min_child -= min_child == s->n
				|| min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
		if (!(min_heap_elem_greater(e, s->p[min_child])))
			break;
		(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}
	min_heap_shift_up_(s, hole_index, e);
}
} /* namespace vos */

