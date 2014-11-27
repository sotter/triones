
#include "cnet.h"
#include "../comm/comlog.h"
#include "tbtimeutil.h"

namespace triones
{

Transport::Transport()
{
	_stop = false;

	_hash_socks = new HashSock(this);
}

Transport::~Transport()
{
	if(_stop)
	{
		stop();
	}

	if(_hash_socks != NULL)
	{
		delete _hash_socks;
		_hash_socks = NULL;
	}
}

// 起动运输层，创建两个线程，一个用于读，一个用写。
bool Transport::start()
{
	signal(SIGPIPE, SIG_IGN );

	_io_thread.start(this, &_sock_event);
	_timeout_thread.start(this, NULL);

	return true;
}

// 停止，停掉读写线程，及销毁。
bool Transport::stop()
{
	_stop = true;
	return wait();
}

// 等待线程完全退出。
bool Transport::wait()
{
	_io_thread.join();
	_timeout_thread.join();
	destroy();
	return true;
}

// socket event 的检测, 被run函数调用
void Transport::event_loop(SocketEvent *socketEvent)
{
	IOEvent events[MAX_SOCKET_EVENTS];

	while (!_stop)
	{
		// 检查是否有事件发生
		int cnt = socketEvent->get_events(1000, events, MAX_SOCKET_EVENTS);

		if (cnt < 0)
		{
			OUT_INFO(NULL, 0, NULL, "get events error, errno %d \n", errno);
		}

		for (int i = 0; i < cnt; i++)
		{
			IOComponent *ioc = events[i]._ioc;
			if (ioc == NULL)
			{
				continue;
			}

			if (events[i]._errorOccurred)
			{
				printf("handle error event %s \n", ioc->info().c_str());
				// 错误发生了
				remove_component(ioc);
				continue;
			}

			ioc->add_ref();
			bool rc = true;
			if (events[i]._readOccurred)
			{
				printf("handle read event %s \n", ioc->info().c_str());
				rc = ioc->handle_read_event();
			}

			if (rc && events[i]._writeOccurred)
			{
				printf("handle write event %s \n", ioc->info().c_str());
				rc = ioc->handle_write_event();
			}
			ioc->sub_ref();

			if (!rc)
			{
				remove_component(ioc);
			}
		}

		//获取被回收且引用计数为0的socket
		HashSock::IOCQueue del;
		IOComponent *ioc = NULL;
		_hash_socks->get_del_list(del);
		while((ioc = del.pop()) != NULL)
		{
			printf("delete ioc %s", ioc->info().c_str());
			printf("==========refcnt = 0, delete IOC ========================\n");
			delete ioc;
			ioc = NULL;
		}
	}
}

//  超时检查, 被run函数调用
void Transport::timeout_loop()
{
//	HashSock::IOCQueue del;
	HashSock::IOCQueue timeout;

	IOComponent *ioc = NULL;
	while (!_stop)
	{
		//检测超时，并超时的队列放入到超时队列中
		_hash_socks->get_timeout_list(timeout);
		while((ioc = timeout.pop()) != NULL)
		{
			printf("timeout_loolp remove ioc %s", ioc->info().c_str());
			remove_component(ioc);
			_hash_socks->moveto_recycle(ioc);
		}

//		//获取被回收且引用计数为0的socket
//		_hash_socks->get_del_list(del);
//		while((ioc = del.pop()) != NULL)
//		{
//			printf("delete ioc %s", ioc->info().c_str());
//			printf("==========refcnt = 0, delete IOC ========================\n");
//			delete ioc;
//			ioc = NULL;
//		}
		usleep(1 * 1000 * 1000);  // 最小间隔1s
	}

	_hash_socks->distroy();
}

/* ***********************************
 * 线程的运行函数，实现Runnable接口中的函数
 * **********************************/
void Transport::run(triones::TBThread *thread, void *arg)
{
	if (thread == &_timeout_thread)
	{
		timeout_loop();
	}
	else
	{
		event_loop((SocketEvent*) arg);
	}
}

// 把[upd|tcp]:ip:port分开放在args中,cnt:数组中最大个数, return返回的数组中个数
int Transport::parse_addr(char *src, char **args, int cnt)
{
	int index = 0;
	char *prev = src;

	while (*src)
	{
		if (*src == ':')
		{
			*src = '\0';
			args[index++] = prev;
			if (index >= cnt)
			{ // 数组满了,返回
				return index;
			}
			prev = src + 1;
		}
		src++;
	}
	args[index++] = prev;
	return index;
}

/*
 * @param spec: 格式 [upd|tcp]:ip:port
 * @param streamer: 数据包的双向流，用packet创建，解包，组包。
 * @param serverAdapter: 用在服务器端，当Connection初始化及Channel创建时回调时用
 * @return IO组件一个对象的指针
 */
IOComponent *Transport::listen(const char *spec, triones::TransProtocol *streamer,
        IServerAdapter *serverAdapter)
{
	char tmp[1024];
	char *args[32];
	strncpy(tmp, spec, 1024);
	tmp[1023] = '\0';

	if (parse_addr(tmp, args, 32) != 3)
	{
		return NULL;
	}

	char *host = args[1];
	int port = atoi(args[2]);

	if (strcasecmp(args[0], "tcp") == 0)
	{
		ServerSocket *socket = new ServerSocket();

		if (!socket->listen(host, port, Socket::TRIONES_SOCK_TCP))
		{
			delete socket;
			return NULL;
		}

		// TCPAcceptor
		TCPAcceptor *acceptor = new TCPAcceptor(this, socket, streamer, serverAdapter);

		if (!acceptor->init())
		{
			delete acceptor;
			return NULL;
		}

		acceptor->setid(socket->get_sockid());
		// 加入到iocomponents中，及注册可读到socketevent中
		add_component(acceptor, true, false);

		// 返回
		return acceptor;
	}
	else if (strcasecmp(args[0], "udp") == 0)
	{
		Socket *socket = new Socket();
		if (!socket->listen(host, port, Socket::TRIONES_SOCK_UDP))
		{
			delete socket;
			OUT_ERROR(NULL, 0, NULL, "udp server bind error: %s:%d, %s", host, port, spec);
			return NULL;
		}

		// TCPAcceptor
		UDPAcceptor *acceptor = new UDPAcceptor(this, socket, streamer, serverAdapter);

		if (!acceptor->init())
		{
			delete acceptor;
			return NULL;
		}

		acceptor->setid(socket->get_sockid());
		// 加入到iocomponents中，及注册可读到socketevent中
		add_component(acceptor, true, false);
		// 返回
		return acceptor;
	}

	return NULL;
}

/*
 * 创建一个Connection，连接到指定的地址，并加入到Socket的监听事件中。
 * @param spec: 格式 [upd|tcp]:ip:port
 * @param streamer: 数据包的双向流，用packet创建，解包，组包。
 * @return  返回一个Connectoion对象指针
 */
IOComponent *Transport::connect(const char *spec, triones::TransProtocol *streamer, bool autoReconn)
{
	char tmp[1024];
	char *args[32];
	strncpy(tmp, spec, 1024);
	tmp[1023] = '\0';

	if (parse_addr(tmp, args, 32) != 3)
	{
		return NULL;
	}

	char *host = args[1];
	unsigned short port = (unsigned short)atoi(args[2]);

	if (strcasecmp(args[0], "tcp") == 0)
	{
		// Socket
		Socket *socket = new Socket();
		if( ! socket->socket_create(Socket::TRIONES_SOCK_TCP)
			|| !socket->set_address(host, port, true))
		{
			delete socket;
			return NULL;
		}

		TCPComponent *component = new TCPComponent(this, socket, streamer,
				NULL, triones::IOComponent::TRIONES_TCPCONN);

		// 设置是否自动重连
		component->set_auto_conn(autoReconn);

		if (!component->init())
		{
			delete component;
			OUT_ERROR(NULL, 0, NULL, "init TCPComponent fail: %s:%d", host, port);
			return NULL;
		}

		add_component(component, true, true);
		return component;
	}
	else if (strcasecmp(args[0], "udp") == 0)
	{
		Socket *socket = new Socket();

		if (!socket->socket_create(Socket::TRIONES_SOCK_UDP)
				|| !socket->set_address(host, port, true))
		{
			delete socket;
			OUT_ERROR(NULL, 0, NULL, "set udp address error: %s:%d, %s", host, port, spec);
			return NULL;
		}

		UDPComponent *component = new UDPComponent(this, socket, streamer, NULL,
		        IOComponent::TRIONES_UDPCONN);

		//对UDP来说， setAddress成功即连接成功
		component->set_state(triones::IOComponent::TRIONES_CONNECTED);

		if (!component->init())
		{
			delete component;
			printf("init updcomponent fail: %s:%d \n", host, port);
			OUT_ERROR(NULL, 0, NULL, "init updcomponent fail: %s:%d", host, port);
			return NULL;
		}

		add_component(component, true, false);

		return component;
	}

	return NULL;
}

/*
 * 加入到iocomponents中
 * @param  ioc: IO组件
 * @param  isWrite: 加入读或写事件到socketEvent中
 */
void Transport::add_component(IOComponent *ioc, bool readOn, bool writeOn)
{
	assert(ioc != NULL);

	if (ioc->is_used())
	{
		OUT_ERROR(NULL, 0, NULL, "already addComponent: %p", ioc);
		return;
	}

	if( _hash_socks->put(ioc))
	{
		ioc->set_used(true);
	}

	// 设置socketevent
	Socket *socket = ioc->get_socket();

	ioc->set_sockevent(&_sock_event);

	if(_sock_event.add_event(socket, readOn, writeOn))
	{
		ioc->add_ref();
	}

	return;
}

// 从Transport的链表管理中将其去掉， 业务层在主动销毁IOC时，可调用removeComponent
// 不能直接调用IOC的 close， disconnect等接口。
// (1) 调用IOC close 函数，清除网络层数据，回调业务层的链路断开。
// (2) 从sock_hash清除，然后将其放入到待回收的删除队列， 定时器检测待删除队列，当引入计数为0时，将其删除。
void Transport::remove_component(IOComponent *ioc)
{
	printf("remove ioc %s \n", ioc->info().c_str());

	assert(ioc != NULL);

	triones::Guard guard(_iocs_mutex);

	// 在这里面已经在sockevent中将其删除了
	// 为什么没有在这里显式的调用sock_event->remove_event，考虑到UDP的情况，并不真正的remove_event;
	ioc->close();

	// 需要重连, 不从iocomponents去掉
	if (ioc->is_auto_conn())
	{
		return;
	}

//  不在iocList中
	if (ioc->is_used())
	{
		_hash_socks->remove(ioc);
		ioc->set_used(false);
	}

	return;
}

void Transport::destroy()
{
	return;
}

bool* Transport::getStop()
{
	return &_stop;
}

}
