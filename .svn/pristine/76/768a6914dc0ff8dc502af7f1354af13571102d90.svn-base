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


namespace triones {

triones::Mutex Socket::_dnsMutex;

/*
 * ���캯��
 */
Socket::Socket() {
    _socketHandle = -1;
}

/*
 * ��������
 */
Socket::~Socket() {
    close();
}

/*
 * ���õ�ַ
 *
 * @param address  host��ip��ַ
 * @param port  �˿ں�
 * @return �Ƿ�ɹ�
 */

bool Socket::setAddress (const char *address, const int port) {
    // ��ʼ��
    memset(static_cast<void *>(&_address), 0, sizeof(_address));

    _address.sin_family = AF_INET;
    _address.sin_port = htons(static_cast<short>(port));

    bool rc = true;
    // �ǿ��ַ����ó�INADDR_ANY

    if (address == NULL || address[0] == '\0') {
        _address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        char c;

        const char *p = address;

        bool isIPAddr = true;

        // ��ip��ַ��ʽ��?
        while ((c = (*p++)) != '\0') {
            if ((c != '.') && (!((c >= '0') && (c <= '9')))) {
                isIPAddr = false;
                break;
            }
        }

        if (isIPAddr) {
            _address.sin_addr.s_addr = inet_addr(address);
        } else {
            // ���������һ��
            _dnsMutex.lock();

            struct hostent *myHostEnt = gethostbyname(address);

            if (myHostEnt != NULL) {
                memcpy(&(_address.sin_addr), *(myHostEnt->h_addr_list),
                       sizeof(struct in_addr));
            } else {
                rc = false;
            }

            _dnsMutex.unlock();
        }
    }

    return rc;
}

/*
 * socket ����Ƿ񴴽�
 */
bool Socket::checkSocketHandle() {
    if (_socketHandle == -1 && (_socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return false;
    }
    return true;
}

/*
 * ���ӵ�_address��
 *
 * @return �Ƿ�ɹ�
 */
bool Socket::connect() {
    if (!checkSocketHandle()) {
        return false;
    }
//    TBSYS_LOG(DEBUG, "��, fd=%d, addr=%s", _socketHandle, getAddr().c_str());
    return (0 == ::connect(_socketHandle, (struct sockaddr *)&_address, sizeof(_address)));
}

/**
 * �ر�����
 */
void Socket::close() {
    if (_socketHandle != -1) {
//        TBSYS_LOG(DEBUG, "�ر�, fd=%d, addr=%s", _socketHandle, getAddr().c_str());
        ::close(_socketHandle);
        _socketHandle = -1;
    }
}

/*
 * �رն�д
 */
void Socket::shutdown() {
    if (_socketHandle != -1) {
        ::shutdown(_socketHandle, SHUT_WR);
    }
}

/**
 * ʹ��UDP��socket
 *
 * @return �Ƿ�ɹ�
 */
bool Socket::createUDP() {
    close();
    _socketHandle = socket(AF_INET, SOCK_DGRAM, 0);
    return (_socketHandle != -1);
}

/*
 * ��socketHandle,��ipaddress���õ���socket��
 *
 * @param  socketHandle: socket���ļ����
 * @param hostAddress: ��������ַ
 */

void Socket::setUp(int socketHandle, struct sockaddr *hostAddress) {
    close();
    _socketHandle = socketHandle;
    memcpy(&_address, hostAddress, sizeof(_address));
}

/*
 * �����ļ����
 *
 * @return �ļ����
 */
int Socket::getSocketHandle() {
    return _socketHandle;
}

/*
 * ����event attribute
 *
 * @return  IOComponent
 */
IOComponent *Socket::getIOComponent() {
    return _iocomponent;
}

/*
 * ����IOComponent
 *
 * @param IOComponent
 */
void Socket::setIOComponent(IOComponent *ioc) {
    _iocomponent = ioc;
}

/*
 * д���
 */
int Socket::write (const void *data, int len) {
    if (_socketHandle == -1) {
        return -1;
    }

    int res;
    do {
        res = ::write(_socketHandle, data, len);
        if (res > 0) {
            //TBSYS_LOG(INFO, "д�����, fd=%d, addr=%d", _socketHandle, res);
            TBNET_COUNT_DATA_WRITE(res);
        }
    } while (res < 0 && errno == EINTR);
    return res;
}

/*
 * �����
 */
int Socket::read (void *data, int len) {
    if (_socketHandle == -1) {
        return -1;
    }

    int res;
    do {
        res = ::read(_socketHandle, data, len);
        if (res > 0) {
            //TBSYS_LOG(INFO, "�������, fd=%d, addr=%d", _socketHandle, res);
            TBNET_COUNT_DATA_READ(res);
        }
    } while (res < 0 && errno == EINTR);
    return res;
}

/*
 * ����int���͵�option
 */
bool Socket::setIntOption (int option, int value) {
    bool rc=false;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, SOL_SOCKET, option,
                         (const void *)(&value), sizeof(value)) == 0);
    }
    return rc;
}

/*
 * ����time���͵�option
 */
bool Socket::setTimeOption(int option, int milliseconds) {
    bool rc=false;
    if (checkSocketHandle()) {
        struct timeval timeout;
        timeout.tv_sec = (int)(milliseconds / 1000);
        timeout.tv_usec = (milliseconds % 1000) * 1000000;
        rc = (setsockopt(_socketHandle, SOL_SOCKET, option,
                         (const void *)(&timeout), sizeof(timeout)) == 0);
    }
    return rc;
}

bool Socket::setSoLinger(bool doLinger, int seconds) {
    bool rc=false;
    struct linger lingerTime;
    lingerTime.l_onoff = doLinger ? 1 : 0;
    lingerTime.l_linger = seconds;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, SOL_SOCKET, SO_LINGER,
                         (const void *)(&lingerTime), sizeof(lingerTime)) == 0);
    }

    return rc;
}

bool Socket::setTcpNoDelay(bool noDelay) {
    bool rc = false;
    int noDelayInt = noDelay ? 1 : 0;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, IPPROTO_TCP, TCP_NODELAY,
                         (const void *)(&noDelayInt), sizeof(noDelayInt)) == 0);
    }
    return rc;
}

bool Socket::setTcpQuickAck(bool quickAck) {
  bool rc = false;
  int quickAckInt = quickAck ? 1 : 0;
  if (checkSocketHandle()) {
    rc = (setsockopt(_socketHandle, IPPROTO_TCP, TCP_QUICKACK,
          (const void *)(&quickAckInt), sizeof(quickAckInt)) == 0);
  }
  return rc;
}

/*
 * �Ƿ�����
 */
bool Socket::setSoBlocking(bool blockingEnabled) {
    bool rc=false;

    if (checkSocketHandle()) {
        int flags = fcntl(_socketHandle, F_GETFL, NULL);
        if (flags >= 0) {
            if (blockingEnabled) {
                flags &= ~O_NONBLOCK; // clear nonblocking
            } else {
                flags |= O_NONBLOCK;  // set nonblocking
            }

            if (fcntl(_socketHandle, F_SETFL, flags) >= 0) {
                rc = true;
            }
        }
    }

    return rc;
}

/*
 * �õ�ip��ַ, д��tmp��
 */
std::string Socket::getAddr() {
    char dest[32];
    unsigned long ad = ntohl(_address.sin_addr.s_addr);
    sprintf(dest, "%d.%d.%d.%d:%d",
            static_cast<int>((ad >> 24) & 255),
            static_cast<int>((ad >> 16) & 255),
            static_cast<int>((ad >> 8) & 255),
            static_cast<int>(ad & 255),
            ntohs(_address.sin_port));
    return dest;
}

/*
 * �õ�64λ���ֵ�ip��ַ
 */
uint64_t Socket::getId() {
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

uint64_t Socket::getPeerId() {
    if (_socketHandle == -1) 
        return 0;

    struct sockaddr_in peer;
    socklen_t length = sizeof(peer);
    if (getpeername(_socketHandle,(struct sockaddr*)&peer, &length) == 0) {
        return ipToAddr(peer.sin_addr.s_addr, ntohs(peer.sin_port));
    }
    return 0;
}

/**
 * �õ����ض˿�
 */
int Socket::getLocalPort() {
    if (_socketHandle == -1) {
        return -1;
    }

    int result = -1;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(_socketHandle, (struct sockaddr*)(&addr), &len) == 0) {
        result = ntohs(addr.sin_port);
    }
    return result;
}

/*
 * �õ�socket����
 */
int Socket::getSoError () {
    if (_socketHandle == -1) {
        return EINVAL;
    }

    int lastError = Socket::getLastError();
    int  soError = 0;
    socklen_t soErrorLen = sizeof(soError);
    if (getsockopt(_socketHandle, SOL_SOCKET, SO_ERROR, (void *)(&soError), &soErrorLen) != 0) {
        return lastError;
    }
    if (soErrorLen != sizeof(soError))
        return EINVAL;

    return soError;
}

}
