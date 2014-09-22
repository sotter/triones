/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : epoll 事件反应器
 */

#ifndef _TEST_IEVENTREACTOR_INC
#define _TEST_IEVENTREACTOR_INC

#include "net/eventreactor.h"
#include "log/comlog.h"

class TestEventReactor: public IEventReactor
{
public:

	virtual ~TestEventReactor()
	{

	}
	virtual int on_read(Socket *sock_item, Packet *packet)
	{
		sock_item->send(packet->data(), packet->size());
		return 0;
	}

	//新连接到来，是否单独一个线程触发，还是转发成一个消息发送给业务处理线程。如果转成消息的话，就不需要此接口。
	//如果全部是消息的方式，那么就全都转成on_event()
	virtual int on_conn(Socket *sock_item)
	{
		OUT_INFO(NULL, 0, NULL, "NEW CLIENT,fd[%d]", sock_item->_fd);
		char send_buf[100] = { "\0" };
		sprintf(send_buf, "wangshitao-pingpang-fd[%d]", sock_item->_fd);
		int len = sizeof(send_buf);
		sock_item->send(send_buf, len);
		return 0;
	}

	//同上，是否单独触发还是修改成消息的问题。
	virtual int on_disconn(Socket *sock_item)
	{
		return 0;
	}
};
#endif
