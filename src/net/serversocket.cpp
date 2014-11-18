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

// accept一个新的连接
Socket *ServerSocket::accept()
{
	Socket *sock = NULL;
	struct sockaddr_in addr;
	int len = sizeof(addr);

	int fd = ::accept(_fd, (struct sockaddr *) &addr, (socklen_t*) &len);

	if (fd >= 0)
	{
		sock = new Socket();
		sock->setup(fd, &_address, (struct sockaddr_in *) &addr);
		OUT_INFO(NULL, 0, NULL, "accept %s , fd %d ", sock->get_peer_addr().c_str(), fd);
	}
	else
	{
		int error = get_last_error();
		if (error != EAGAIN)
		{
			OUT_ERROR(NULL, 0, "accept error %s(%d)", strerror(error), error);
		}
	}

	return sock;
}

} /* namespace triones */
