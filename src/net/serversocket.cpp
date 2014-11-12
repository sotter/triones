/******************************************************
 *   FileName: ServerSocket.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "../comm/comlog.h"

namespace triones
{

ServerSocket::ServerSocket()
{
	_back_log = 256;
}

ServerSocket::~ServerSocket()
{

}
/*
 * accept一个新的连接
 *
 * @return 一个Socket
 */
Socket *ServerSocket::accept()
{
	Socket *handleSocket = NULL;

	struct sockaddr_in addr;
	int len = sizeof(addr);

	int fd = ::accept(_fd, (struct sockaddr *) &addr, (socklen_t*) &len);

	if (fd >= 0)
	{
		handleSocket = new Socket();
		handleSocket->setup(fd, (struct sockaddr *) &addr);
		OUT_INFO(NULL, 0, NULL, "accept %s , fd %d ", handleSocket->get_addr().c_str(), fd);
	}
	else
	{
		int error = get_last_error();
		if (error != EAGAIN)
		{
			OUT_ERROR(NULL, 0, "accept error %s(%d)", strerror(error), error);
		}
	}

	return handleSocket;
}

/*
 * 打开监听
 *
 * @return 是否成功
 */
bool ServerSocket::listen()
{
	if (!check_fd())
	{
		return false;
	}

	// 地址可重用
	set_solinger(false, 0);
	set_reuse_addr(true);
	set_int_option(SO_KEEPALIVE, 1);
	set_int_option(SO_SNDBUF, 640000);
	set_int_option(SO_RCVBUF, 640000);
	set_tcp_nodelay(true);

	if (::bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0)
	{
		OUT_INFO(NULL, 0, NULL, "bind %s error : %d", this->get_addr().c_str(), errno);
		return false;
	}

	if (::listen(_fd, _back_log) < 0)
	{
		OUT_INFO(NULL, 0, NULL, "listen %s error : %d", this->get_addr().c_str(), errno);
		return false;
	}

	return true;
}

} /* namespace triones */
