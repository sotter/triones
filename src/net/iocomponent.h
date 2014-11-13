/******************************************************
 *   FileName: IOComponent.h
 *     Author: triones  2014-9-18
 * IOC的引用技术规则：当其他的对象聚合IOC对象指针的时候引用计数加1，解决聚合时引用技术减1，主要有以下三个场景
 * （1）epoll事件监测时 add_event, remove_event
 * （2）解包形成的packet，要告知上层 是哪个IOC产生的。packet产生后加1，delete后减1
 * （3）业务层USER绑定IOC
 * （4）其他需要绑定IOC的场景
 *******************************************************/

#ifndef IOCOMPONENT_H_
#define IOCOMPONENT_H_

namespace triones
{

class IOComponent
{
public:
	enum
	{
		TRIONES_CONNECTING = 1, TRIONES_CONNECTED, TRIONES_CLOSED, TRIONES_UNCONNECTED
	};

	enum
	{
		TRIONES_TCPACCETOR = 1,  // TCP listen socket
		TRIONES_TCPCONN,         // TCP 主动发起连接的socket
		TRIONES_TCPACTCONN,      // TCP 被动连接的socket，由Acceptor派生出来的
		TRIONES_UDPACCETOR,      // UDP 服务端socket
		TRIONES_UDPCONN,         // UDP 主动起连接的socket
		TRIONES_UDPACTCONN       // UDP server派生出来的component
	};

	friend class Transport;

public:

	IOComponent(Transport *owner, Socket *socket, int type = 0, uint64_t id = 0);

	virtual ~IOComponent();

	//socket event 写事件处理
	virtual bool handle_read_event() = 0;

	//socket event 读事件处理
	virtual bool handle_write_event() = 0;

	//超时检测, return false:未超时正常，true超时需要清理
	virtual bool check_timeout(uint64_t now) = 0;

	//由子类实现，非必须实现的接口
	virtual bool post_packet(Packet *packet);

	//IOComponent 网络资源清理
	virtual void close() = 0;

	//设置可写
	void enable_write(bool on);

public:

	Socket *get_socket()
	{
		return _socket;
	}

	uint64_t get_last_use_time()
	{
		return _last_use_time;
	}

	triones::Transport *get_owner()
	{
		return _owner;
	}

	void set_sockevent(SocketEvent *socketEvent)
	{
		_sock_event = socketEvent;
	}

	// 为了适配baseService的接口而增加的  2014-10-14
	void set_server_adapter(IServerAdapter *sa)
	{
		_server_adapter = sa;
	}

	int add_ref()
	{
		return atomic_add_return(1, &_refcount);
	}

	void sub_ref()
	{
		atomic_dec(&_refcount);
	}

	int get_ref()
	{
		return atomic_read(&_refcount);
	}

	bool ref_none()
	{
		return atomic_read(&_refcount) <= 0;
	}

	//设置IOC的ID
	void setid(uint64_t id)
	{
		_id = id;
	}

	//获取IOC的ID
	uint64_t getid()
	{
		return _id;
	}

	// 是否连接状态, 包括正在连接
	bool is_conn_state()
	{
		return (_state == TRIONES_CONNECTED || _state == TRIONES_CONNECTING);
	}

	void set_state(int state)
	{
		_state = state;
	}

	// 得到连接状态
	int get_state()
	{
		return _state;
	}

	// 设置是否重连
	void set_auto_conn(bool on)
	{
		_auto_reconn = on;
	}

	// 得到重连标志
	bool is_auto_conn()
	{
		return (_auto_reconn);
	}


	// 是否在hash_sock中
	bool is_used()
	{
		return _inused;
	}

	// 设置是否被用
	void set_used(bool b)
	{
		_inused = b;
	}

	int get_type()
	{
		return _type;
	}

public:

	IServerAdapter *_server_adapter;

	IOComponent *_pre;
	IOComponent *_next;

protected:
	//对于服务端产生的socket，_id为socket的本端ADDRESS ID
	//对于服务端产生的socket，_id为socket的对端ADDRESS ID
	uint64_t  _id;

	//绑定的socket句柄，系统所产生的所有socket都有IOC绑定，一个socket可以同时被多个IOC绑定例如UDPACTCONN多个共享一个socket
	Socket *_socket;

	//IOC所属的Transport
	triones::Transport *_owner;

	//IOC复用句柄；todo：IOC实际用不着，可以删除
	SocketEvent *_sock_event;

	// IOC类型
	int _type;

	//当前的连接状态
	int _state;

	// 引用计数
	atomic_t _refcount;

	//是否重连标志
	bool _auto_reconn;

	//是否在用，指的是是否在transport的hashsock中
	bool _inused;

	// 最近使用的系统时间
	uint64_t _last_use_time;
};
} /* namespace triones */
#endif /* IOCOMPONENT_H_ */
