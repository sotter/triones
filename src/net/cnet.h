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
#include <time.h>

#include <list>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#include <sys/types.h>
#include <arpa/inet.h>

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
class HashSock;
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
#include "sockutil.h"
#include "tbtimeutil.h"

#include "sockutil.h"
#include "socket.h"
#include "serversocket.h"
#include "socketevent.h"

#include "iocomponent.h"
#include "tcpacceptor.h"
#include "tcpcomponent.h"
#include "udpacceptor.h"
#include "udpcomponent.h"
#include "hashioc.h"
#include "transport.h"
#include "baseservice.h"

#define __DEBUG_INFO__

#ifdef __DEBUG_INFO__
#define __INTO_FUN__  printf("into %s\n", __FUNCTION__);
#else
#define __INTO_FUN__   ;
#endif

#endif

#endif /* CNET_H_ */
