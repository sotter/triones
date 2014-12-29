#include "cnet.h"
#include <sys/poll.h>
#include "stats.h"
#include "../comm/comlog.h"

namespace triones
{

triones::Mutex Socket::_dns_mutex;

Socket::Socket()
{
	memset(&_address, 0, sizeof(_address));
	memset(&_peer_address, 0, sizeof(_peer_address));

	_fd = -1;
	_setup = 0;
	_type = 0;
}

Socket::~Socket()
{
	close();
}

// 连接到_address上
bool Socket::connect(const char *host, const unsigned short port, int type)
{
	if (!socket_create(type)) return false;

	if (!set_address(host, port, true))
	{
		OUT_INFO(NULL, 0, NULL, "connect set address error, host : %s, port : %d\n", host, port);
		return false;
	}

	return (0 == ::connect(_fd, (struct sockaddr *) &_peer_address, sizeof(_peer_address)));
}

// 建立ACCEPTOR套结字，对于UDP来说仅仅是绑定address，没有listen的过程
bool Socket::listen(const char *host, const unsigned short port, int type)
{
	if (!socket_create(type)) return false;

	if (!set_address(host, port, false))
	{
		OUT_INFO(NULL, 0, NULL, "connect set address error, host : %s, port : %d\n", host, port);
		return false;
	}

	set_reuse_addr(true);

	//不论TCP还是UDP读写缓冲区都设置为64K
	set_int_option(SO_SNDBUF, 64 * 1024);
	set_int_option(SO_RCVBUF, 64 * 1024);
	set_so_blocking(false);

	if (type == TRIONES_SOCK_TCP)
	{
		// 地址可重用
		set_solinger(false, 0);
		set_tcp_nodelay(true);
		set_int_option(SO_KEEPALIVE, 1);

		if (::bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0)
		{
			OUT_INFO(NULL, 0, NULL, "tcp server bind %s error : %d", this->get_addr().c_str(),
			        errno);
			return false;
		}

		if (::listen(_fd, 1024) < 0)
		{
			OUT_INFO(NULL, 0, NULL, "tpc server listen %s error : %d", this->get_addr().c_str(),
			        errno);
			return false;
		}
	}
	else
	{
		if (::bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0)
		{
			OUT_INFO(NULL, 0, NULL, "tcp server bind %s error : %d", this->get_addr().c_str(),
			        errno);
			return false;
		}
	}

	return true;
}

bool Socket::setup(int fd, struct sockaddr_in *addr, struct sockaddr_in *peer_addr, bool tcp)
{
	//在fd没有初始化，和 _iocomponent尚未空的情况下，是不能执行setup操作的。
	if (fd <= 0) return false;

	if (_fd > 0 && _fd != fd)
	{
		close();
	}

	_fd = fd;

	if (!set_address(addr, false) || !set_address(peer_addr, true))
	{
		return false;
	}

	if(tcp) _setup |= TCP_FLAG;

	return true;
}

bool Socket::socket_create(int type)
{
	if(_fd > 0) return true;

	if (type == TRIONES_SOCK_TCP)
	{
		_fd = socket(AF_INET, SOCK_STREAM, 0);
		_setup |= TCP_FLAG;
	}
	else
	{
		_fd = socket(AF_INET, SOCK_DGRAM, 0);
		_setup &= ~TCP_FLAG;
	}

	if(_fd < 0)
	{
		OUT_ERROR(NULL, 0, NULL, "socket create error : %s(%d)", strerror(errno), errno);
	}

	return _fd > 0;
}

bool Socket::set_address(const char *host, unsigned short port, bool peer)
{
	int type = (_setup & TCP_FLAG) ? TRIONES_SOCK_TCP : TRIONES_SOCK_UDP;
	if(!socket_create(type))
		return false;

	struct sockaddr_in *dest = peer ? (&_peer_address) : (&_address);

	memset(dest, 0, sizeof(struct sockaddr_in));

	dest->sin_family = AF_INET;
	dest->sin_port = htons(static_cast<short>(port));

	bool rc = true;
	if (host == NULL || host[0] == '\0')
	{
		dest->sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		char c;
		const char *p = host;
		bool is_ipaddr = true;
		while ((c = (*p++)) != '\0')
		{
			if ((c != '.') && (!((c >= '0') && (c <= '9'))))
			{
				is_ipaddr = false;
				break;
			}
		}

		if (is_ipaddr)
		{
			dest->sin_addr.s_addr = inet_addr(host);
		}
		else
		{
			_dns_mutex.lock();
			struct hostent *host_ent = gethostbyname(host);
			if (host_ent != NULL)
			{
				memcpy(&(dest->sin_addr), *(host_ent->h_addr_list), sizeof(struct in_addr));
			}
			else
			{
				rc = false;
			}
			_dns_mutex.unlock();
		}
	}

	if(rc)
	{
		_setup |= peer ? PEERINIT_FLAG : ADDRINIT_FLAG;
	}

	return rc;
}

//优先级：
//(1) 外部传入的addr不为空，已外部传入的addr准
//(2) 外部传入为空时，已通过系统调用获取的addr为准
//(3) 在无法获取_fd的sockname时，已原先的为标准
bool Socket::set_address(struct sockaddr_in *addr, bool peer)
{
	int type = (_setup & TCP_FLAG) ? TRIONES_SOCK_TCP : TRIONES_SOCK_UDP;
	if(!socket_create(type))
		return false;

	uint8_t init_flag = peer ? PEERINIT_FLAG : ADDRINIT_FLAG;

	struct sockaddr_in *dest = peer ?  &_peer_address : &_address;
	memset(dest, 0, sizeof(struct sockaddr_in));

	if(addr != NULL)
	{
		memcpy((void*)dest, addr, sizeof(struct sockaddr_in));
		//设置已经初始化标识
		_setup |= init_flag;
		return true;
	}
	else
	{
		//如果可以获取_fd的peer addr, 那么以取到的为标准，如果取不到（如tcp连接未建立时）
		//那么以原先有的为标准
		struct sockaddr_in temp;
		socklen_t length = sizeof(struct sockaddr_in);

		if(peer)
		{
			if(getpeername(_fd, (struct sockaddr*)&temp, &length) == 0)
			{
				memcpy((void*) dest, &temp, sizeof(struct sockaddr_in));
				//设置已经初始化标识
				_setup |= init_flag;
			}
		}else
		{
			if(getsockname(_fd, (struct sockaddr*) &temp, &length) == 0)
			{
				memcpy((void*) dest, &temp, sizeof(struct sockaddr_in));
				//设置已经初始化标识
				_setup |= init_flag;
			}
		}

		return true;
	}
}

bool Socket::connect()
{
	int type = (_setup & TCP_FLAG) ? TRIONES_SOCK_TCP : TRIONES_SOCK_UDP;
	if(!socket_create(type))
		return false;

	return (0 == ::connect(_fd, (struct sockaddr *) &_peer_address, sizeof(_peer_address)));
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
	if (_fd == -1) return -1;

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

int Socket::nonblock_read(void *data, int len)
{
	if (_fd == -1) return -1;

	int res;
	do
	{
		res = ::recv(_fd, data, len, MSG_DONTWAIT);
		if (res > 0)
		{
			TBNET_COUNT_DATA_READ(res);
		}
	} while (res < 0 && errno == EINTR);

	return res;
}

int Socket::recvfrom(void *data, int len, sockaddr_in &src)
{
	if (_fd == -1) return -1;

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

int Socket::nonblock_recvfrom(void *data, int len, sockaddr_in &src)
{
	if (_fd == -1) return -1;

	int res;
	int addr_len = sizeof(sockaddr_in);

	do
	{
		res = ::recvfrom(_fd, (void*) data, len, MSG_DONTWAIT, (struct sockaddr *) &src,
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
	return (setsockopt(_fd, SOL_SOCKET, option, (const void *) (&value), sizeof(value)) == 0);
}

bool Socket::set_time_option(int option, int milliseconds)
{
	struct timeval timeout;
	timeout.tv_sec = (int) (milliseconds / 1000);
	timeout.tv_usec = (milliseconds % 1000) * 1000000;
	return (setsockopt(_fd, SOL_SOCKET, option, (const void *) (&timeout), sizeof(timeout)) == 0);
}

bool Socket::set_keep_alive(bool on)
{
	return set_int_option(SO_KEEPALIVE, on ? 1 : 0);
}

bool Socket::set_reuse_addr(bool on)
{
	return set_int_option(SO_REUSEADDR, on ? 1 : 0);
}

bool Socket::set_solinger(bool on, int seconds)
{
	struct linger linger_time;
	linger_time.l_onoff = on ? 1 : 0;
	linger_time.l_linger = seconds;
	return (setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const void *) (&linger_time),
	        sizeof(linger_time)) == 0);
}

bool Socket::set_tcp_nodelay(bool nodelay)
{
	int noDelayInt = nodelay ? 1 : 0;
	return (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (const void *) (&noDelayInt),
	        sizeof(noDelayInt)) == 0);
}

bool Socket::set_tcp_quick_ack(bool quick_ack)
{
	int quickAckInt = quick_ack ? 1 : 0;
	return (setsockopt(_fd, IPPROTO_TCP, TCP_QUICKACK, (const void *) (&quickAckInt),
	        sizeof(quickAckInt)) == 0);
}

bool Socket::set_so_blocking(bool on)
{
	bool rc = false;

	int flags = fcntl(_fd, F_GETFL, NULL);
	if (flags >= 0)
	{
		if (on)
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

	return rc;
}

std::string Socket::get_addr()
{
	char dest[32];
	unsigned long ad = ntohl(_address.sin_addr.s_addr);
	const char *type = (_setup & TCP_FLAG) ? "tcp" : "udp";
	sprintf(dest, "%s:%d.%d.%d.%d:%d", type, static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(_address.sin_port));
	return dest;
}

std::string Socket::get_peer_addr()
{
	char dest[32];
	unsigned long ad = ntohl(_peer_address.sin_addr.s_addr);
	const char *type = (_setup & TCP_FLAG) ? "tcp" : "udp";
	sprintf(dest, "%s:%d.%d.%d.%d:%d", type, static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(_peer_address.sin_port));
	return dest;
}

uint64_t Socket::get_sockid(bool is_tcp)
{
	if (_fd == -1) return 0;

	if(_iocomponent->get_type() == IOComponent::TRIONES_TCPCONN
			|| _iocomponent->get_type() == IOComponent::TRIONES_UDPCONN)
	{
		return sockutil::sock_addr2id(&_address, is_tcp, false);
	}
	else
	{
		return sockutil::sock_addr2id(&_address, is_tcp, true);
	}
}

uint64_t Socket::get_peer_sockid(bool is_tcp)
{
	if (_fd == -1) return 0;

	if(_iocomponent->get_type() == IOComponent::TRIONES_TCPCONN
			|| _iocomponent->get_type() == IOComponent::TRIONES_UDPCONN)
	{
		return sockutil::sock_addr2id(&_peer_address, is_tcp, false);
	}
	else
	{
		return sockutil::sock_addr2id(&_peer_address, is_tcp, true);
	}

	return 0;
}

int Socket::get_soerror()
{
	if (_fd == -1) return EINVAL;

	int last_error = Socket::get_last_error();
	int soerror = 0;
	socklen_t soerror_len = sizeof(soerror);
	if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *) (&soerror), &soerror_len) != 0)
	{
		return last_error;
	}

	if (soerror_len != sizeof(soerror)) return EINVAL;

	return soerror;
}

void Socket::show_addr()
{
	struct sockaddr_in address, peer_address;

	memset(&address, 0, sizeof(address));
	memset(&peer_address, 0, sizeof(peer_address));

	socklen_t length = sizeof(address);
	socklen_t length2 = sizeof(peer_address);

	if (_setup & ADDRINIT_FLAG)
	{
		memcpy(&address, &_address, sizeof(struct sockaddr_in));
	}
	else
	{
		if (getsockname(_fd, (struct sockaddr*) &address, &length) != 0)
		{
			OUT_ERROR(NULL, 0, NULL, "getsockname error %s \n", strerror(errno));
		}
	}

	if (_setup & PEERINIT_FLAG)
	{
		memcpy(&peer_address, &_peer_address, sizeof(struct sockaddr_in));
	}
	else
	{
		if (getpeername(_fd, (struct sockaddr*) &peer_address, &length2) != 0)
		{
			OUT_ERROR(NULL, 0, NULL, "getpeername error %s \n", strerror(errno));
		}
	}

	//const char *type = _setup & TCP_FLAG ? "tcp" : "udp";

//	printf("local address : %s:%s peer address : %s:%s \n", type,
//			sockutil::sock_addr2str(&address).c_str(), type,
//			sockutil::sock_addr2str(&peer_address).c_str());

	return ;
}

}
