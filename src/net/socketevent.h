/******************************************************
 *   FileName: SocketEvent.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef SOCKETEVENT_H_
#define SOCKETEVENT_H_

#define MAX_EPOLL_EVENT         0xffffff  //初始支持的socket数目
#define MAX_SOCKET_EVENTS       10240     //每次能检测的最大数目
namespace triones
{
class IOEvent
{

public:
	bool _readOccurred;
	bool _writeOccurred;
	bool _errorOccurred;
	IOComponent *_ioc;
};

class SocketEvent
{
public:
	SocketEvent();
	virtual ~SocketEvent();

	bool add_event(Socket *socket, bool enableRead, bool enable_write);

	bool set_event(Socket *socket, bool enableRead, bool enable_write);

	bool remove_event(Socket *socket);

	int get_events(int timeout, IOEvent *events, int cnt);

private:

	int _iepfd;
};

} /* namespace triones */
#endif /* SOCKETEVENT_H_ */
