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
	// TODO Auto-generated constructor stub
}

SocketEvent::~SocketEvent()
{
	// TODO Auto-generated destructor stub
}

SocketEvent::SocketEvent() {
    _iepfd = epoll_create(MAX_EPOLL_EVENT);
}

/*
 * 析造函数
 */
SocketEvent::~SocketEvent() {
    close(_iepfd);
}

/*
 * 增加Socket到事件中
 *
 * @param socket 被加的socket
 * @param enableRead: 设置是否可读
 * @param enable_write: 设置是否可写
 * @return  操作是否成功, true – 成功, false – 失败
 */
bool SocketEvent::addEvent(Socket *socket, bool enableRead, bool enable_write)
{

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = socket->getIOComponent();
    // 设置要处理的事件类型
    ev.events = 0;

    if (enableRead) {
        ev.events |= EPOLLIN;
    }
    if (enable_write) {
        ev.events |= EPOLLOUT;
    }

    //_mutex.lock();
    bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_ADD, socket->getSocketHandle(), &ev) == 0);
    //_mutex.unlock();
    //TBSYS_LOG(ERROR, "EPOLL_CTL_ADD: %d => %d,%d, %d", socket->getSocketHandle(), enableRead, enable_write, pthread_self());
    return rc;
}

/*
 * 设置删除Socket到事件中
 *
 * @param socket 被加的socket
 * @param enableRead: 设置是否可读
 * @param enable_write: 设置是否可写
 * @return  操作是否成功, true – 成功, false – 失败
 */
bool SocketEvent::setEvent(Socket *socket, bool enableRead, bool enable_write)
{

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = socket->getIOComponent();
    // 设置要处理的事件类型
    ev.events = 0;

    if (enableRead) {
        ev.events |= EPOLLIN;
    }
    if (enable_write) {
        ev.events |= EPOLLOUT;
    }

    //_mutex.lock();
    bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_MOD, socket->getSocketHandle(), &ev) == 0);
    //_mutex.unlock();
    //TBSYS_LOG(ERROR, "EPOLL_CTL_MOD: %d => %d,%d, %d", socket->getSocketHandle(), enableRead, enable_write, pthread_self());
    return rc;
}

/*
 * 删除Socket到事件中
 *
 * @param socket 被删除socket
 * @return  操作是否成功, true – 成功, false – 失败
 */
bool SocketEvent::removeEvent(Socket *socket)
{

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = socket->getIOComponent();
    // 设置要处理的事件类型
    ev.events = 0;
    //_mutex.lock();
    bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_DEL, socket->getSocketHandle(), &ev) == 0);
    //_mutex.unlock();
    //TBSYS_LOG(ERROR, "EPOLL_CTL_DEL: %d", socket->getSocketHandle());
    return rc;
}

/*
 * 得到读写事件。
 *
 * @param timeout  超时时间(单位:ms)
 * @param events  事件数组
 * @param cnt   events的数组大小
 * @return 事件数, 0为超时, -1为出错了
 */
int SocketEvent::getEvents(int timeout, IOEvent *ioevents, int cnt)
{
    struct epoll_event events[MAX_SOCKET_EVENTS];

    if (cnt > MAX_SOCKET_EVENTS) {
        cnt = MAX_SOCKET_EVENTS;
    }

    int res = epoll_wait(_iepfd, events, cnt , timeout);

    // 初始化
    if (res > 0) {
        memset(ioevents, 0, sizeof(IOEvent) * res);
    }

    // 把events的事件转化成IOEvent的事件
    for (int i = 0; i < res; i++) {
        ioevents[i]._ioc = (IOComponent*)events[i].data.ptr;
        if (events[i].events & (EPOLLERR | EPOLLHUP)) {
            ioevents[i]._errorOccurred = true;
        }
        if ((events[i].events & EPOLLIN) != 0) {
            ioevents[i]._readOccurred = true;
        }
        if ((events[i].events & EPOLLOUT) != 0) {
            ioevents[i]._writeOccurred = true;
        }
    }

    return res;
}

} /* namespace triones */
