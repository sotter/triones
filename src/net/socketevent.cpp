/******************************************************
 *   FileName: SocketEvent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "socketevent.h"

namespace triones
{

SocketEvent::SocketEvent()
{
	_iepfd = epoll_create(MAX_EPOLL_EVENT);
}

SocketEvent::~SocketEvent()
{
	close(_iepfd);
}

// 增加Socket到事件中
bool SocketEvent::add_event(Socket *socket, bool enableRead, bool enable_write)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.ptr = socket->get_ioc();
	// 设置要处理的事件类型
	ev.events = 0;

	if (enableRead)
	{
		ev.events |= EPOLLIN;
	}
	if (enable_write)
	{
		ev.events |= EPOLLOUT;
	}

	//_mutex.lock();
	bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_ADD, socket->get_fd(), &ev) == 0);
	//_mutex.unlock();
	return rc;
}

// 设置删除Socket到事件中
bool SocketEvent::set_event(Socket *socket, bool enableRead, bool enable_write)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.ptr = socket->get_ioc();
	// 设置要处理的事件类型
	ev.events = 0;

	if (enableRead)
	{
		ev.events |= EPOLLIN;
	}
	if (enable_write)
	{
		ev.events |= EPOLLOUT;
	}

	//_mutex.lock();
	bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_MOD, socket->get_fd(), &ev) == 0);
	//_mutex.unlock();
	//TBSYS_LOG(ERROR, "EPOLL_CTL_MOD: %d => %d,%d, %d", socket->getSocketHandle(), enableRead, enable_write, pthread_self());
	return rc;
}

// 删除Socket到事件中
bool SocketEvent::remove_event(Socket *socket)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.ptr = socket->get_ioc();
	// 设置要处理的事件类型
	ev.events = 0;
	//_mutex.lock();
	bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_DEL, socket->get_fd(), &ev) == 0);
	//_mutex.unlock();
	//TBSYS_LOG(ERROR, "EPOLL_CTL_DEL: %d", socket->getSocketHandle());
	return rc;
}

/****************************
EPOLLHUP事件触发：当socket的一端认为对方发来了一个不存在的4元组请求的时候,会回复一个RST响应,在epoll上会响应为EPOLLHUP事件
[1] 当客户端向一个没有在listen的服务器端口发送的connect的时候 服务器会返回一个RST 因为服务器根本不知道这个4元组的存在.
[2] 当已经建立好连接的一对客户端和服务器,客户端突然操作系统崩溃,或者拔掉电源导致操作系统重新启动(kill pid或者正常关机不行的,因为操作系统会发送FIN给对方).
这时服务器在原有的4元组上发送数据,会收到客户端返回的RST,因为客户端根本不知道之前这个4元组的存在.
 ****************************/
int SocketEvent::get_events(int timeout, IOEvent *ioevents, int cnt)
{
	struct epoll_event events[MAX_SOCKET_EVENTS];

	if (cnt > MAX_SOCKET_EVENTS)
	{
		cnt = MAX_SOCKET_EVENTS;
	}

	int res = epoll_wait(_iepfd, events, cnt, timeout);

	// 初始化
	if (res > 0)
	{
		memset(ioevents, 0, sizeof(IOEvent) * res);
	}

	// 把events的事件转化成IOEvent的事件
	for (int i = 0; i < res; i++)
	{
		ioevents[i]._ioc = (IOComponent*) events[i].data.ptr;
		if (events[i].events & (EPOLLERR | EPOLLHUP))
		{
#ifdef __DEBUG_INFO__
			printf("%s %d res = %d EPOLLERR|EPOLLHUP \n", __FILE__, __LINE__, res);
#endif
			ioevents[i]._errorOccurred = true;
		}
		if ((events[i].events & EPOLLIN) != 0)
		{
#ifdef __DEBUG_INFO__
			printf("%s %d res = %d EPOLLIN \n", __FILE__, __LINE__, res);
#endif
			ioevents[i]._readOccurred = true;
		}
		if ((events[i].events & EPOLLOUT) != 0)
		{
#ifdef __DEBUG_INFO__
			printf("%s %d res = %d EPOLLOUT \n", __FILE__, __LINE__, res);
#endif
			ioevents[i]._writeOccurred = true;
		}
	}
	return res;
}

} /* namespace triones */
