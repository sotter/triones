
#include "cnet.h"
#include <sys/poll.h>
#include "stats.h"
#include "../comm/comlog.h"

namespace triones
{

triones::Mutex Socket::_dnsMutex;

Socket::Socket()
{
	_fd = -1;
}

Socket::~Socket()
{
	close();
}

bool Socket::set_address(const char *address, const int port)
{
	memset(static_cast<void *>(&_address), 0, sizeof(_address));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(static_cast<short>(port));

	bool rc = true;

	if (address == NULL || address[0] == '\0')
	{
		_address.sin_addr.s_addr = htonl(INADDR_ANY );
	}
	else
	{
		char c;

		const char *p = address;

		bool isIPAddr = true;

		while ((c = (*p++)) != '\0')
		{
			if ((c != '.') && (!((c >= '0') && (c <= '9'))))
			{
				isIPAddr = false;
				break;
			}
		}

		if (isIPAddr)
		{
			_address.sin_addr.s_addr = inet_addr(address);
		}
		else
		{
			_dnsMutex.lock();

			struct hostent *myHostEnt = gethostbyname(address);

			if (myHostEnt != NULL)
			{
				memcpy(&(_address.sin_addr), *(myHostEnt->h_addr_list), sizeof(struct in_addr));
			}
			else
			{
				rc = false;
			}

			_dnsMutex.unlock();
		}
	}

	return rc;
}

bool Socket::udp_bind()
{
	if (!check_fd())
	{
		return false;
	}

	// 地址可重用
	set_reuse_addr(true);
	set_int_option(SO_SNDBUF, 640000);
	set_int_option(SO_RCVBUF, 640000);

	if (::bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0)
	{
		OUT_INFO(NULL, 0, NULL, "bind %s error : %d", this->get_addr().c_str(), errno);
		return false;
	}

	return true;
}

bool Socket::check_fd()
{
	if (_fd == -1 && (_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return false;
	}
	return true;
}

bool Socket::connect()
{
	if (!check_fd())
	{
		return false;
	}
	return (0 == ::connect(_fd, (struct sockaddr *) &_address, sizeof(_address)));
}

void Socket::close()
{
	if (_fd != -1)
	{
		::close(_fd);
		_fd = -1;
	}
}

void Socket::shutdown()
{
	if (_fd != -1)
	{
		::shutdown(_fd, SHUT_WR);
	}
}

bool Socket::udp_create()
{
	close();
	_fd = socket(AF_INET, SOCK_DGRAM, 0);
	return (_fd != -1);
}

void Socket::setup(int socketHandle, struct sockaddr *hostAddress)
{
	close();
	_fd = socketHandle;
	memcpy(&_address, hostAddress, sizeof(_address));
}

int Socket::get_fd()
{
	return _fd;
}

IOComponent *Socket::get_ioc()
{
	return _iocomponent;
}

void Socket::set_ioc(IOComponent *ioc)
{
	_iocomponent = ioc;
}

int Socket::write(const void *data, int len)
{
	if (_fd == -1)
	{
		return -1;
	}

	int res;
	do
	{
		res = ::write(_fd, data, len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_WRITE(res);
		}
	} while (res < 0 && errno == EINTR);
	return res;
}

int Socket::sendto(const void *data, int len, sockaddr_in &dest)
{
	if (_fd == -1)
	{
		return -1;
	}

	int res;
	int addr_len = sizeof(sockaddr_in);
	do
	{
		res = ::sendto(_fd, data, len, 0, (struct sockaddr *) &dest, addr_len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_WRITE(res);
		}

	} while (res < 0 && errno == EINTR);

	return res;
}

int Socket::read(void *data, int len)
{
	if (_fd == -1) return -1;

	int res;
	do
	{
		res = ::read(_fd, data, len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_READ(res);
		}
	} while (res < 0 && errno == EINTR);

	return res;
}

int Socket::recvfrom(void *data, int len, sockaddr_in &src)
{
	if (_fd == -1)
	{
		return -1;
	}

	int res;
	int addr_len = sizeof(sockaddr_in);

	do
	{
		res = ::recvfrom(_fd, (void*) data, len, 0, (struct sockaddr *) &src,
		        (socklen_t*) &addr_len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_READ(res);
		}
	} while (res < 0 && errno == EINTR);

	return res;
}

bool Socket::set_int_option(int option, int value)
{
	bool rc = false;
	if (check_fd())
	{
		rc = (setsockopt(_fd, SOL_SOCKET, option, (const void *) (&value), sizeof(value))
		        == 0);
	}
	return rc;
}

bool Socket::set_time_option(int option, int milliseconds)
{
	bool rc = false;
	if (check_fd())
	{
		struct timeval timeout;
		timeout.tv_sec = (int) (milliseconds / 1000);
		timeout.tv_usec = (milliseconds % 1000) * 1000000;
		rc = (setsockopt(_fd, SOL_SOCKET, option, (const void *) (&timeout),
		        sizeof(timeout)) == 0);
	}
	return rc;
}

bool Socket::set_keep_alive(bool on)
{
	return set_int_option(SO_KEEPALIVE, on ? 1 : 0);
}

bool Socket::set_reuse_addr(bool on)
{
	return set_int_option(SO_REUSEADDR, on ? 1 : 0);
}

bool Socket::set_solinger(bool doLinger, int seconds)
{
	bool rc = false;
	struct linger lingerTime;
	lingerTime.l_onoff = doLinger ? 1 : 0;
	lingerTime.l_linger = seconds;
	if (check_fd())
	{
		rc = (setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const void *) (&lingerTime),
		        sizeof(lingerTime)) == 0);
	}

	return rc;
}

bool Socket::set_tcp_nodelay(bool noDelay)
{
	bool rc = false;
	int noDelayInt = noDelay ? 1 : 0;
	if (check_fd())
	{
		rc = (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (const void *) (&noDelayInt),
		        sizeof(noDelayInt)) == 0);
	}
	return rc;
}

bool Socket::set_tcp_quick_ack(bool quickAck)
{
	bool rc = false;
	int quickAckInt = quickAck ? 1 : 0;
	if (check_fd())
	{
		rc = (setsockopt(_fd, IPPROTO_TCP, TCP_QUICKACK, (const void *) (&quickAckInt),
		        sizeof(quickAckInt)) == 0);
	}
	return rc;
}

bool Socket::set_so_blocking(bool blockingEnabled)
{
	bool rc = false;

	if (check_fd())
	{
		int flags = fcntl(_fd, F_GETFL, NULL);
		if (flags >= 0)
		{
			if (blockingEnabled)
			{
				flags &= ~O_NONBLOCK; // clear nonblocking
			}
			else
			{
				flags |= O_NONBLOCK;  // set nonblocking
			}

			if (fcntl(_fd, F_SETFL, flags) >= 0)
			{
				rc = true;
			}
		}
	}

	return rc;
}

std::string Socket::get_addr()
{
	char dest[32];
	unsigned long ad = ntohl(_address.sin_addr.s_addr);
	sprintf(dest, "%d.%d.%d.%d:%d", static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(_address.sin_port));
	return dest;
}

uint64_t Socket::get_sockid()
{
	if (_fd == -1) return 0;
	return sockutil::sock_addr2id(&_address);
}

uint64_t Socket::get_peer_sockid()
{
	if (_fd == -1) return 0;

	struct sockaddr_in peeraddr;
	socklen_t length = sizeof(peeraddr);
	if (getpeername(_fd, (struct sockaddr*) &peeraddr, &length) == 0)
	{
		return sockutil::sock_addr2id(&peeraddr);
	}
	return 0;
}

int Socket::get_soerror()
{
	if (_fd == -1)
	{
		return EINVAL;
	}

	int lastError = Socket::get_last_error();
	int soError = 0;
	socklen_t soErrorLen = sizeof(soError);
	if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *) (&soError), &soErrorLen) != 0)
	{
		return lastError;
	}
	if (soErrorLen != sizeof(soError)) return EINVAL;

	return soError;
}

}
