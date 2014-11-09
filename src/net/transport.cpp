/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */
#include "cnet.h"
#include "../comm/comlog.h"
#include "tbtimeutil.h"

namespace triones
{

Transport::Transport()
{
	_stop = false;
	_iocListHead = _iocListTail = NULL;
	_delListHead = _delListTail = NULL;
	_iocListChanged = false;
	_iocListCount = 0;
}

Transport::~Transport()
{
	destroy();
}

//  起动运输层，创建两个线程，一个用于读，一个用写。
bool Transport::start()
{
	signal(SIGPIPE, SIG_IGN );
	_readWriteThread.start(this, &_socketEvent);
	_timeoutThread.start(this, NULL);
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
	_readWriteThread.join();
	_timeoutThread.join();
	destroy();
	return true;
}

// socket event 的检测, 被run函数调用
void Transport::eventLoop(SocketEvent *socketEvent)
{
	IOEvent events[MAX_SOCKET_EVENTS];

	while (!_stop)
	{
		// 检查是否有事件发生
		int cnt = socketEvent->getEvents(1000, events, MAX_SOCKET_EVENTS);

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
				// 错误发生了
				removeComponent(ioc);
				continue;
			}

			ioc->addRef();
			bool rc = true;
			if (events[i]._readOccurred)
			{
				rc = ioc->handleReadEvent();
			}

			if (rc && events[i]._writeOccurred)
			{
				rc = ioc->handleWriteEvent();
			}

			ioc->subRef();

			if (!rc)
			{
				removeComponent(ioc);
			}
		}
	}
}

//  超时检查, 被run函数调用
void Transport::timeoutLoop()
{
	IOComponent *mydelHead = NULL;
	IOComponent *mydelTail = NULL;
	std::vector<IOComponent*> mylist;
	while (!_stop)
	{
		// 先写复制到list中
		_iocsMutex.lock();
		if (_iocListChanged)
		{
			mylist.clear();
			IOComponent *iocList = _iocListHead;
			while (iocList)
			{
				mylist.push_back(iocList);
				iocList = iocList->_next;
			}
			_iocListChanged = false;
		}

		// 加入到mydel中
		if (_delListHead != NULL && _delListTail != NULL)
		{
			if (mydelTail == NULL)
			{
				mydelHead = _delListHead;
			}
			else
			{
				mydelTail->_next = _delListHead;
				_delListHead->_pre = mydelTail;
			}
			mydelTail = _delListTail;
			// 清空delList
			_delListHead = _delListTail = NULL;
		}
		_iocsMutex.unlock();

		// 对每个iocomponent进行检查
		for (int i = 0; i < (int) mylist.size(); i++)
		{
			IOComponent *ioc = mylist[i];
			ioc->checkTimeout(triones::CTimeUtil::getTime());
		}

		// 删除掉
		IOComponent *tmpList = mydelHead;
		int64_t nowTime = triones::CTimeUtil::getTime() - static_cast<int64_t>(900000000); // 15min
		while (tmpList)
		{
			if (tmpList->getRef() <= 0)
			{
				tmpList->subRef();
			}
			if (tmpList->getRef() <= -10 || tmpList->getLastUseTime() < nowTime)
			{
				// 从链中删除
				if (tmpList == mydelHead)
				{
					// head
					mydelHead = tmpList->_next;
				}
				if (tmpList == mydelTail)
				{
					// tail
					mydelTail = tmpList->_pre;
				}
				if (tmpList->_pre != NULL) tmpList->_pre->_next = tmpList->_next;
				if (tmpList->_next != NULL) tmpList->_next->_pre = tmpList->_pre;

				IOComponent *ioc = tmpList;
				tmpList = tmpList->_next;
				OUT_INFO(NULL, 0, NULL, "DELIOC, %s, IOCount:%d, IOC:%p\n",
				        ioc->getSocket()->getAddr().c_str(), _iocListCount, ioc);

				delete ioc;
			}
			else
			{
				tmpList = tmpList->_next;
			}
		}

		usleep(1000000);  // 最小间隔1s
	}

	// 写回到_delList上,让destroy销毁
	_iocsMutex.lock();
	if (mydelHead != NULL)
	{
		if (_delListTail == NULL)
		{
			_delListHead = mydelHead;
		}
		else
		{
			_delListTail->_next = mydelHead;
			mydelHead->_pre = _delListTail;
		}
		_delListTail = mydelTail;
	}
	_iocsMutex.unlock();
}

/* ***********************************
 * 线程的运行函数，实现Runnable接口中的函数
 * **********************************/
void Transport::run(triones::TBThread *thread, void *arg)
{
	if (thread == &_timeoutThread)
	{
		timeoutLoop();
	}
	else
	{
		eventLoop((SocketEvent*) arg);
	}
}

/*
 * 把[upd|tcp]:ip:port分开放在args中
 *
 * @param src: 源格式
 * @param args: 目标数组
 * @param   cnt: 数组中最大个数
 * @return  返回的数组中个数
 */
int Transport::parseAddr(char *src, char **args, int cnt)
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

	if (parseAddr(tmp, args, 32) != 3)
	{
		return NULL;
	}

	char *host = args[1];
	int port = atoi(args[2]);

	if (strcasecmp(args[0], "tcp") == 0)
	{
		// Server Socket
		ServerSocket *socket = new ServerSocket();
		if (!socket->setAddress(host, port))
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

		// 加入到iocomponents中，及注册可读到socketevent中
		addComponent(acceptor, true, false);

		// 返回
		return acceptor;
	}
	else if (strcasecmp(args[0], "udp") == 0)
	{
		Socket *socket = new Socket();
		socket->createUDP();
		if (!socket->setAddress(host, port))
		{
			delete socket;
			OUT_ERROR(NULL, 0, NULL, "设置setAddress错误: %s:%d, %s", host, port, spec);
			return NULL;
		}

		// TCPAcceptor
		UDPAcceptor *acceptor = new UDPAcceptor(this, socket, streamer, serverAdapter);

		if (!acceptor->init())
		{
			delete acceptor;
			return NULL;
		}

		// 加入到iocomponents中，及注册可读到socketevent中
		addComponent(acceptor, true, false);

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

	if (parseAddr(tmp, args, 32) != 3)
	{
		return NULL;
	}

	char *host = args[1];
	int port = atoi(args[2]);

	if (strcasecmp(args[0], "tcp") == 0)
	{
		// Socket
		Socket *socket = new Socket();

		if (!socket->setAddress(host, port))
		{
			delete socket;
			OUT_ERROR(NULL, 0, NULL, "设置setAddress错误: %s:%d, %s", host, port, spec);
			return NULL;
		}

		// TCPComponent
		TCPComponent *component = new TCPComponent(this, socket, streamer, NULL);
		// 设置是否自动重连
		component->setAutoReconn(autoReconn);
		if (!component->init())
		{
			delete component;
			OUT_ERROR(NULL, 0, NULL, "初始化失败TCPComponent: %s:%d", host, port);
			return NULL;
		}

		addComponent(component, true, true);
		component->addRef();
		return component;
	}
	else if (strcasecmp(args[0], "udp") == 0)
	{
		Socket *socket = new Socket();
		socket->createUDP();
		if (!socket->setAddress(host, port))
		{
			delete socket;
			OUT_ERROR(NULL, 0, NULL, "set udp address error: %s:%d, %s", host, port, spec);
			return NULL;
		}

		UDPComponent *component = new UDPComponent(this, socket, streamer, NULL,
		        IOComponent::TRIONES_UDPCONN);

		if (!component->init())
		{
			delete component;
			printf("init updcomponent fail: %s:%d \n", host, port);
			OUT_ERROR(NULL, 0, NULL, "init updcomponent fail: %s:%d", host, port);
			return NULL;
		}

		//UDP写操作采用同步行为，所有的UDP ioc都不接收写事件。
		addComponent(component, true, false);
		component->addRef();
		return component;
	}

	return NULL;
}

bool Transport::disconnect(TCPComponent *conn)
{
	IOComponent *ioc = NULL;
	if (conn == NULL)
	{
		return false;
	}
	ioc->setAutoReconn(false);
	ioc->subRef();
	if (ioc->_socket)
	{
		ioc->_socket->shutdown();
	}
	return true;
}

/*
 * 加入到iocomponents中
 * @param  ioc: IO组件
 * @param  isWrite: 加入读或写事件到socketEvent中
 */
void Transport::addComponent(IOComponent *ioc, bool readOn, bool writeOn)
{
	assert(ioc != NULL);

	_iocsMutex.lock();
	if (ioc->isUsed())
	{
		OUT_ERROR(NULL, 0, NULL, "已给加过addComponent: %p", ioc);
		_iocsMutex.unlock();
		return;
	}
	// 加入iocList上
	ioc->_pre = _iocListTail;
	ioc->_next = NULL;
	if (_iocListTail == NULL)
	{
		_iocListHead = ioc;
	}
	else
	{
		_iocListTail->_next = ioc;
	}
	_iocListTail = ioc;
	// 设置在用
	ioc->setUsed(true);
	_iocListChanged = true;
	_iocListCount++;
	_iocsMutex.unlock();

	// 设置socketevent
	Socket *socket = ioc->getSocket();
	ioc->setSocketEvent(&_socketEvent);
	_socketEvent.addEvent(socket, readOn, writeOn);

	OUT_INFO(NULL, 0, NULL, "ADDIOC, SOCK: %d, %s, RON: %d, WON: %d, IOCount:%d, IOC:%p\n",
	        socket->getSocketHandle(), ioc->getSocket()->getAddr().c_str(), readOn, writeOn,
	        _iocListCount, ioc);
}

//从Transport的链表管理中将其去掉， 业务层在主动销毁IOC时，可调用removeComponent
//不能直接调用IOC的 close， disconnect等接口。
/* (1) 调用IOC close 函数，清除网络层数据，回调业务层的链路断开。
 * (2) 从sock_hash清除，然后将其放入到待回收的删除队列， 定时器检测待删除队列，当引入计数为0时，将其删除。  */
void Transport::removeComponent(IOComponent *ioc)
{
	assert(ioc != NULL);
	triones::Guard guard(_iocsMutex);

	ioc->close();

	// 需要重连, 不从iocomponents去掉
	if (ioc->isAutoReconn())
	{
		return;
	}

	// 不在iocList中
	if (ioc->isUsed() == false)
	{
		return;
	}

	// 从_iocList删除
	if (ioc == _iocListHead)
	{
		_iocListHead = ioc->_next;
	}

	if (ioc == _iocListTail)
	{
		_iocListTail = ioc->_pre;
	}

	if (ioc->_pre != NULL) ioc->_pre->_next = ioc->_next;
	if (ioc->_next != NULL) ioc->_next->_pre = ioc->_pre;

	// 加入到_delList
	ioc->_pre = _delListTail;
	ioc->_next = NULL;
	if (_delListTail == NULL)
	{
		_delListHead = ioc;
	}
	else
	{
		_delListTail->_next = ioc;
	}
	_delListTail = ioc;

	// 引用计数减一
	ioc->setUsed(false);
	_iocListChanged = true;
	_iocListCount--;

	OUT_INFO(NULL, 0, NULL, "RMIOC, %s IOCount:%d, IOC:%p\n", ioc->getSocket()->getAddr().c_str(),
	        _iocListCount, ioc);
}

void Transport::destroy()
{
	triones::Guard guard(_iocsMutex);

	IOComponent *list, *ioc;
	// 删除iocList
	list = _iocListHead;
	while (list)
	{
		ioc = list;
		list = list->_next;
		_iocListCount--;
		OUT_INFO(NULL, 0, NULL, "DELIOC, IOCount:%d, IOC:%p\n", _iocListCount, ioc);
		delete ioc;
	}
	_iocListHead = _iocListTail = NULL;
	_iocListCount = 0;
	// 删除delList
	list = _delListHead;
	while (list)
	{
		ioc = list;
		assert(ioc != NULL);
		list = list->_next;
		OUT_INFO(NULL, 0, NULL, "DELIOC, IOCount:%d, IOC:%p\n", _iocListCount, ioc);
		delete ioc;
	}
	_delListHead = _delListTail = NULL;
}

bool* Transport::getStop()
{
	return &_stop;
}

}
