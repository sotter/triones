/******************************************************
 *   FileName: cnet.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef CNET_H_
#define CNET_H_

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#include <sys/types.h>

namespace triones
{
class TimeUtil;
class Thread;
class Runnable;
class DataBuffer;

class Packet;
class IServerAdapter;
class PacketQueue;

class Socket;
class ServerSocket;
class IOEvent;
class SocketEvent;
class IOComponent;
class TCPAcceptor;
class TCPComponent;
class Transport;
class UDPAcceptor;
class UDPComponent;

class PacketQueueThread;
class ConnectionManager;
}

#ifndef UNUSED
#define UNUSED(v) ((void)(v))

#include "atomic.h"
#include "iserveradapter.h"

#include "socket.h"
#include "serversocket.h"
#include "socketevent.h"

#include "iocomponent.h"
#include "tcpacceptor.h"
#include "tcpcomponent.h"
#include "udpacceptor.h"
#include "udpcomponent.h"
#include "transport.h"
#include "baseservice.h"

// Define C99 equivalent types.
//typedef signed char int8_t;
//typedef signed short int16_t;
//typedef signed int int32_t;
////typedef signed long long int64_t;
//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;
//typedef unsigned long long uint64_t;



#endif

#endif /* CNET_H_ */
