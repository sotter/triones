/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#include "cnet.h"
#include <sys/poll.h>
#include "stats.h"
#include "../comm/comlog.h"

namespace triones
{

triones::Mutex Socket::_dnsMutex;

Socket::Socket()
{
	_socketHandle = -1;
}

Socket::~Socket()
{
	close();
}

bool Socket::setAddress(const char *address, const int port)
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

		// ��ip��ַ��ʽ��?
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

bool Socket::udpBind()
{
	if (!checkSocketHandle())
	{
		return false;
	}

	// 地址可重用
	setReuseAddress(true);
	setIntOption(SO_SNDBUF, 640000);
	setIntOption(SO_RCVBUF, 640000);

	if (::bind(_socketHandle, (struct sockaddr *) &_address, sizeof(_address)) < 0)
	{
		OUT_INFO(NULL, 0, NULL, "bind %s error : %d", this->getAddr().c_str(), errno);
		return false;
	}

	return true;
}

bool Socket::checkSocketHandle()
{
	if (_socketHandle == -1 && (_socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return false;
	}
	return true;
}

bool Socket::connect()
{
	if (!checkSocketHandle())
	{
		return false;
	}
	return (0 == ::connect(_socketHandle, (struct sockaddr *) &_address, sizeof(_address)));
}

void Socket::close()
{
	if (_socketHandle != -1)
	{
		::close(_socketHandle);
		_socketHandle = -1;
	}
}

void Socket::shutdown()
{
	if (_socketHandle != -1)
	{
		::shutdown(_socketHandle, SHUT_WR);
	}
}

bool Socket::createUDP()
{
	close();
	_socketHandle = socket(AF_INET, SOCK_DGRAM, 0);
	return (_socketHandle != -1);
}

void Socket::setUp(int socketHandle, struct sockaddr *hostAddress)
{
	close();
	_socketHandle = socketHandle;
	memcpy(&_address, hostAddress, sizeof(_address));
}

int Socket::getSocketHandle()
{
	return _socketHandle;
}

IOComponent *Socket::getIOComponent()
{
	return _iocomponent;
}

void Socket::setIOComponent(IOComponent *ioc)
{
	_iocomponent = ioc;
}

int Socket::write(const void *data, int len)
{
	if (_socketHandle == -1)
	{
		return -1;
	}

	int res;
	do
	{
		res = ::write(_socketHandle, data, len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_WRITE(res);
		}
	} while (res < 0 && errno == EINTR);
	return res;
}

int Socket::sendto(const void *data, int len, sockaddr_in &dest)
{
	if (_socketHandle == -1)
	{
		return -1;
	}

	int res;
	int addr_len = sizeof(sockaddr_in);
	do
	{
		res = ::sendto(_socketHandle, data, len, 0, (struct sockaddr *) &dest, addr_len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_WRITE(res);
		}

	} while (res < 0 && errno == EINTR);

	return res;
}

int Socket::read(void *data, int len)
{
	if (_socketHandle == -1) return -1;

	int res;
	do
	{
		res = ::read(_socketHandle, data, len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_READ(res);
		}
	} while (res < 0 && errno == EINTR);

	return res;
}

int Socket::recvfrom(void *data, int len, sockaddr_in &src)
{
	if (_socketHandle == -1)
	{
		return -1;
	}

	int res;
	int addr_len = sizeof(sockaddr_in);

	do
	{
		res = ::recvfrom(_socketHandle, (void*) data, len, 0, (struct sockaddr *) &src,
		        (socklen_t*) &addr_len);
		if (res > 0)
		{
			TBNET_COUNT_DATA_READ(res);
		}
	} while (res < 0 && errno == EINTR);

	return res;
}

bool Socket::setIntOption(int option, int value)
{
	bool rc = false;
	if (checkSocketHandle())
	{
		rc = (setsockopt(_socketHandle, SOL_SOCKET, option, (const void *) (&value), sizeof(value))
		        == 0);
	}
	return rc;
}

bool Socket::setTimeOption(int option, int milliseconds)
{
	bool rc = false;
	if (checkSocketHandle())
	{
		struct timeval timeout;
		timeout.tv_sec = (int) (milliseconds / 1000);
		timeout.tv_usec = (milliseconds % 1000) * 1000000;
		rc = (setsockopt(_socketHandle, SOL_SOCKET, option, (const void *) (&timeout),
		        sizeof(timeout)) == 0);
	}
	return rc;
}

bool Socket::setSoLinger(bool doLinger, int seconds)
{
	bool rc = false;
	struct linger lingerTime;
	lingerTime.l_onoff = doLinger ? 1 : 0;
	lingerTime.l_linger = seconds;
	if (checkSocketHandle())
	{
		rc = (setsockopt(_socketHandle, SOL_SOCKET, SO_LINGER, (const void *) (&lingerTime),
		        sizeof(lingerTime)) == 0);
	}

	return rc;
}

bool Socket::setTcpNoDelay(bool noDelay)
{
	bool rc = false;
	int noDelayInt = noDelay ? 1 : 0;
	if (checkSocketHandle())
	{
		rc = (setsockopt(_socketHandle, IPPROTO_TCP, TCP_NODELAY, (const void *) (&noDelayInt),
		        sizeof(noDelayInt)) == 0);
	}
	return rc;
}

bool Socket::setTcpQuickAck(bool quickAck)
{
	bool rc = false;
	int quickAckInt = quickAck ? 1 : 0;
	if (checkSocketHandle())
	{
		rc = (setsockopt(_socketHandle, IPPROTO_TCP, TCP_QUICKACK, (const void *) (&quickAckInt),
		        sizeof(quickAckInt)) == 0);
	}
	return rc;
}

bool Socket::setSoBlocking(bool blockingEnabled)
{
	bool rc = false;

	if (checkSocketHandle())
	{
		int flags = fcntl(_socketHandle, F_GETFL, NULL);
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

			if (fcntl(_socketHandle, F_SETFL, flags) >= 0)
			{
				rc = true;
			}
		}
	}

	return rc;
}

std::string Socket::getAddr()
{
	char dest[32];
	unsigned long ad = ntohl(_address.sin_addr.s_addr);
	sprintf(dest, "%d.%d.%d.%d:%d", static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(_address.sin_port));
	return dest;
}

uint64_t Socket::getId()
{
	uint64_t ip = ntohs(_address.sin_port);
	ip <<= 32;
	ip |= _address.sin_addr.s_addr;
	return ip;
}

uint64_t ipToAddr(uint32_t ip, int port)
{
	uint64_t ipport = port;
	ipport <<= 32;
	ipport |= ip;
	return ipport;
}

uint64_t Socket::getPeerId()
{
	if (_socketHandle == -1) return 0;

	struct sockaddr_in peer;
	socklen_t length = sizeof(peer);
	if (getpeername(_socketHandle, (struct sockaddr*) &peer, &length) == 0)
	{
		return ipToAddr(peer.sin_addr.s_addr, ntohs(peer.sin_port));
	}
	return 0;
}

int Socket::getLocalPort()
{
	if (_socketHandle == -1)
	{
		return -1;
	}

	int result = -1;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (getsockname(_socketHandle, (struct sockaddr*) (&addr), &len) == 0)
	{
		result = ntohs(addr.sin_port);
	}
	return result;
}

int Socket::getSoError()
{
	if (_socketHandle == -1)
	{
		return EINVAL;
	}

	int lastError = Socket::getLastError();
	int soError = 0;
	socklen_t soErrorLen = sizeof(soError);
	if (getsockopt(_socketHandle, SOL_SOCKET, SO_ERROR, (void *) (&soError), &soErrorLen) != 0)
	{
		return lastError;
	}
	if (soErrorLen != sizeof(soError)) return EINVAL;

	return soError;
}

}
