/******************************************************
 *   FileName: TCPComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "tcpcomponent.h"
#include "tbtimeutil.h"
#include "../comm/comlog.h"
#include "stats.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define TBNET_MAX_TIME (1ll<<62)

namespace triones
{

TCPComponent::TCPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *adapter, int type)
		: IOComponent(owner, socket, type)
{
	UNUSED(type);
	_start_conn_time = 0;
//	_is_server = false;
	_socket = socket;
	_streamer = streamer;
	_server_adapter = adapter;
	_write_finish_close = false;
	OUT_INFO(NULL, 0, NULL, "+++++TCPComponent");
}

//析构函数
TCPComponent::~TCPComponent()
{
	__INTO_FUN__

//	disconnect();
	if (_socket)
	{
		_socket->close();
		delete _socket;
		_socket = NULL;
	}
	OUT_INFO(NULL, 0, NULL, ">>>>>TCPComponent");
	printf("-----TCPComponent\n");
}

//连接断开，降所有发送队列中的packet全部超时
void TCPComponent::disconnect()
{
	__INTO_FUN__

	_output_mutex.lock();
	_my_queue.moveto(&_output_queue);
	_output_mutex.unlock();

	//业务层的回调这个回调时需要排队的
	if(_server_adapter != NULL)
	{
		_server_adapter->syn_handle_packet(this, new Packet(IServerAdapter::CMD_DISCONN_PACKET));
	}
}


//连接到指定的机器, isServer: 是否初始化一个服务器的Connection
bool TCPComponent::init()
{
	__INTO_FUN__

	_socket->set_so_blocking(false);
	/* 在发送完缓冲区中的数据后，跟正常的TCP连接终止序列，套接字接收缓冲区中的数据被丢弃 */
	_socket->set_solinger(false, 0);
	_socket->set_reuse_addr(true);
	_socket->set_int_option(SO_KEEPALIVE, 1);
	_socket->set_int_option(SO_SNDBUF, 64 * 1024);
	_socket->set_int_option(SO_RCVBUF, 64 * 1024);
	_socket->set_tcp_nodelay(true);

	if (get_type() == IOComponent::TRIONES_TCPCONN)
	{
		if (!socket_connect() && !_auto_reconn)
		{
			return false;
		}
	}
	else
	{
		set_state(TRIONES_CONNECTED);
	}

	if(_socket->setup(_socket->get_fd()))
	{
		//对于tcp客户端来说取得是本端的地址
		if(get_type() == IOComponent::TRIONES_TCPCONN)
		{
			this->setid(_socket->get_sockid());
		}
		//
		else if(get_type() == IOComponent::TRIONES_TCPACTCONN)
		{
			this->setid(_socket->get_peer_sockid());
		}

		return true;
	}

	return false;
}

//  连接到socket
bool TCPComponent::socket_connect()
{
	__INTO_FUN__

	//注意，这个函数必须是可重入的，可能有不同用户的线程调用这个接口，完全依靠state判断也不是很严密的；2014-10-11
	if (get_state() == TRIONES_CONNECTED || get_state() == TRIONES_CONNECTING)
	{
		return true;
	}

	_socket->set_so_blocking(false);

	_start_conn_time = triones::CTimeUtil::get_time();

	// 链接错误(即非EINPROGRESS也非EWOULDBLOCK)
	if (!_socket->connect()) {
		int error = Socket::get_last_error();
		if (error != EINPROGRESS && error != EWOULDBLOCK) {
			_socket->close();
			set_state(TRIONES_CLOSED);
			OUT_ERROR(NULL, 0, NULL, "connect %s fail, %s(%d)", _socket->get_peer_addr().c_str(),
					strerror(error), error);

			return false;
		}
	}

	// 为了统一起见,这里不关心连接是成功还是EINPROGRESS或EWOULDBLOCK，都注册事件，让事件处理线程来统一地处理连接成功或失败操作
	set_state(TRIONES_CONNECTING);
	if (_sock_event)
	{
		_sock_event->add_event(_socket, true, true);
	}
	return true;


//	if (_socket->connect())
//	{
//		OUT_INFO(NULL, 0, NULL, "connect %s success \n", _socket->get_peer_addr().c_str());
//
//		printf("connect %s success \n", _socket->get_peer_addr().c_str());
//
//		if (_sock_event)
//		{
//			_sock_event->add_event(_socket, true, true);
//		}
//		set_state(TRIONES_CONNECTED);
//	}
//	else
//	{
//		int error = Socket::get_last_error();
//		if (error == EINPROGRESS || error == EWOULDBLOCK)
//		{
//			printf("connect %s, EINPROGRESS waiting result \n", _socket->get_peer_addr().c_str());
//			set_state(TRIONES_CONNECTING);
//			if (_sock_event)
//			{
//				_sock_event->add_event(_socket, true, true);
//			}
//		}
//		else
//		{
//			_socket->close();
//			set_state(TRIONES_CLOSED);
//			OUT_ERROR(NULL, 0, NULL, "connect %s fail, %s(%d)", _socket->get_peer_addr().c_str(),
//			        strerror(error), error);
//
//			return false;
//		}
//	}
//	return true;
}

/* **********************************************
 * IOComponent的实现函数，供Transport::removeComponent调用
 * removeComponent的触发条件为检测到网络断开。
 * 由业务层触发的关闭应怎么来实现呢？
 * **********************************************/
void TCPComponent::close()
{
	__INTO_FUN__

	if(get_state() == TRIONES_CLOSED)
		return;

	//如果是异步连接的socket，需要检测关闭的原因， 如果TRIONES_CONNECTING同时还会回调onPacket
	if (get_state() == TRIONES_CONNECTING)
	{
		int error = _socket->get_soerror();
		OUT_ERROR(NULL, 0, NULL, "connect %s fail: %s(%d)", _socket->get_peer_addr().c_str(),
		        strerror(error), error);

		printf("connect %s fail: %s(%d), local addr %s \n", _socket->get_peer_addr().c_str(),
				 strerror(error), error, _socket->get_addr().c_str());
	}

	// 移除事件
	if (_sock_event)
	{
		_sock_event->remove_event(_socket);
	}

	//只有 TRIONES_CONNECTED 和 TRIONES_CONNECTING会有回调
	if (is_conn_state())
	{
		set_state(TRIONES_CLOSED);
		disconnect();
	}

	//将socket真正的关闭
	_socket->close();

	clear_input_buffer();
}

//处理socket写事件
bool TCPComponent::handle_write_event()
{
	__INTO_FUN__

	_last_use_time = triones::CTimeUtil::get_time();
	bool rc = true;

	if (get_state() == TRIONES_CONNECTED)
	{
		rc = write_data();
	}
	else if (get_state() == TRIONES_CONNECTING)
	{
		int error = _socket->get_soerror();

		if (error == 0)
		{
			enable_write(true);
			clear_output_buffer();
			printf("connect %s success \n", _socket->get_peer_addr().c_str());
			OUT_INFO(NULL, 0, NULL, "connect %s success", _socket->get_peer_addr().c_str());
			set_state(TRIONES_CONNECTED);
		}
		else
		{
			OUT_ERROR(NULL, 0, NULL, "connect %s fail: %s(%d)", _socket->get_peer_addr().c_str(),
			        strerror(error), error);
			printf("connect %s fail: %s(%d) \n", _socket->get_peer_addr().c_str(),
			        strerror(error), error);

			if (_sock_event)
			{
				_sock_event->remove_event(_socket);
			}
			_socket->close();
			set_state(TRIONES_CLOSED);
		}

		// 调用回调函数
		if (_server_adapter) {
			bool succ = error == 0 ? true : false;
			_server_adapter->handle_connected(this, succ);
		}
	}

	return rc;
}

// 当有数据可读时被Transport调用
bool TCPComponent::handle_read_event()
{
	__INTO_FUN__
	_last_use_time = triones::CTimeUtil::get_time();
	bool rc = false;
	if (get_state() == TRIONES_CONNECTED)
	{
		rc = read_data();
	}
	return rc;
}

//超时检查
bool TCPComponent::check_timeout(uint64_t now)
{
	printf("into TCPComponent::check_timeout %s\n", info().c_str());

	//client connect的超时时间
	const uint64_t conn_timeout = (2 * 1000 * 1000);
	//重连时间间隔
	const uint64_t reconn_time = (10 * 1000 * 1000);
	//普通socket没有数据时的超时时间
	const uint64_t timeout = (18 * 1000 * 1000);

	bool ret = false;
	// 检查是否连接超时
	if (get_state() == TRIONES_CONNECTING)
	{
		if (_start_conn_time > 0 && _start_conn_time < (now - conn_timeout))
		{
			// 连接超时 2 秒
			set_state(TRIONES_CLOSED);
			OUT_ERROR(NULL, 0, NULL, "connect to %s timeout", _socket->get_addr().c_str());
			//关闭写端
			_socket->shutdown();

			if (get_type() != TRIONES_TCPCONN || !_auto_reconn)
			{
				// 非自动重连，则delete掉ioc
				ret = true;
			}
		}
	}

	else if (get_state() == TRIONES_CONNECTED)
	{
		//客户端不主动做超时检测，只要服务端不给断就一直连接着
		if (get_type() == TRIONES_TCPACTCONN)
		{
			// 连接的时候, 只用在服务器端
			uint64_t last_use_time = _last_use_time;	// 因为_last_use_time随时在变，所以将其值拷贝出来
			if (last_use_time < now - timeout)
			{
				uint64_t idle = now - last_use_time;
//				// 空闲10s 断开
//				set_state(TRIONES_CLOSED);
				OUT_INFO(NULL, 0, NULL, "%s %d(s) with no data, disconnect it",
				        _socket->get_peer_addr().c_str(), (idle / static_cast<uint64_t>(1000000)));

				//流程：本端shutdown后，对端收到收到可读事件后，判定关闭。然后本端收到EPOLLERR|EPOLLHUP事件，触发关闭。
				//todo: 调用shutdown会触发一个可读事件吗，触发可读事件然后将其销毁， 但是UDP这个问题该怎么处理呢？ 2014-11-04
				printf("TRIONES_TCPACTCONN TRIONES_CONNECTED timeout now(%"PRIu64") last_use_time(%"PRIu64") tiemout(%"PRIu64")==============\n", now, _last_use_time, timeout);
				_socket->shutdown();

				// 服务端检测连接空闲时间过长，则delete掉ioc
				ret = true;
			}
		}
	}

	//需要重连的socket
	else if (get_state() == TRIONES_CLOSED)
	{
		printf("_state == TRIONES_CLOSED\n");

		if (get_type() == TRIONES_TCPCONN && _auto_reconn)
		{
			//每隔五秒钟重连一次
			if (_start_conn_time > 0 && _start_conn_time < (now - reconn_time))
			{
				//不管是否连接成功，都更新连接时间，时间间隔都是5000000
				_start_conn_time = triones::CTimeUtil::get_time();
				socket_connect();
			}
		}
		else
		{
			// 非自动重连，则delete掉ioc
			ret = true;
		}
	}
	return ret;
}

///**** 原先connectiong的部分 *********************/

/*** 说明 2014-09-21
 * （1）这个地方应该限定写入次数，如果在单线程的条件下，写入的数据量过大，导致其它的socket的任务一直处于等待状态 ，
 * 在这里面默认最大是只写10次的。
 * （2）对于写事件中出现的异常，如EPIPE等，没有作特殊处理，这样的话只能通过读事件来判定socket的断开情况。
 * （3）将packet序列化为流数据的方式。在淘宝的框架中，所有的通信内部都是采用同一种协议，
 * 用同一种协议进行了封装，数据传输都是通过packet的方式。这个packet是带有chanelID的，这样发送端和接收端的业务就能对应起来。
 * 而在我们的业务中，实现的是跟通用客户端的通信，这种客户端可能不cnet写的，而是走的协议（transprotocol）
 *  *********/
bool TCPComponent::write_data()
{
	__INTO_FUN__
	// 把 _outputQueue copy到 _myQueue中, 从_myQueue中向外发送

	Packet *packet;
	int ret;
	int writeCnt = 0;
	int myQueueSize = 0;

	{
		_output_mutex.lock();
		_output_queue.moveto(&_my_queue);

		//如果socket中的数据已经全部发送完毕了，置可写事件为false，然后退出来
		if (_my_queue.size() == 0 && _output.getDataLen() == 0)
		{
			this->enable_write(false);
			_output_mutex.unlock();
			return true;
		}

		// myQueueSize = _my_queue.size();
		_output_mutex.unlock();
	}



	//todo:这块代码需要重写，packet本身就是从_output继承过来的
	//如果write出现ERRORAGAIN的情况，下一个包继续发送；
	do
	{
		while (_output.getDataLen() < READ_WRITE_SIZE)
		{
			// 队列空了就退出
			//if (myQueueSize == 0) break;
			packet = _my_queue.pop();
			if (packet == NULL) {
				break;
			}

			//为什么将packet放入到_output中发送，如果发送有失败的情况，可以将未发送的数据放入到out_put中；
			//而且是易扩展，如果packet不是继承DataBuffer,可以将packet序列成数据流。
			//缺点是增加了一次内存拷贝。
			_output.writeBytes(packet->getData(), packet->getDataLen());
			myQueueSize--;
			delete packet;
			TBNET_COUNT_PACKET_WRITE(1);
		}

		if (_output.getDataLen() == 0)
		{
			break;
		}

		// write data
		ret = _socket->write(_output.getData(), _output.getDataLen());
		if (ret > 0)
		{
			_output.drainData(ret);
		}
		writeCnt++;
		/*******
		 * _output.getDataLen() == 0 说明发送的数据都结束了
		 * 停止发送的条件：
		 * (1)发送的结果ret <= 0, 发送失败，或者写缓冲区已经满了。
		 * （2） _output.getDataLen() > 0 说明一次没有发送完，还有没有发送完的数据。就直接退出来停止发送了。
		 * 	 那么这块数据去了哪里？
		 * (3)最终myqueue和output中未发送完的数据都到哪里去了
		 **********/
	} while (ret > 0 && _output.getDataLen() == 0 && packet != NULL && writeCnt < 10);

	// 紧缩
	_output.shrink();

	_output_mutex.lock();
	int queueSize = _output_queue.size() + _my_queue.size() + (_output.getDataLen() > 0 ? 1 : 0);
	if (queueSize == 0 || _write_finish_close)
	{
		this->enable_write(false);
	}
	_output_mutex.unlock();

	if (_write_finish_close)
	{
		//主动将队列断开
		OUT_ERROR(NULL, 0, NULL, "initiate to terminate the connection");
		return false;
	}

	return true;
}

/****************************
 *(1)socket能够读取的最大的包是多少
 ***************************/
bool TCPComponent::read_data()
{
	__INTO_FUN__
	//每个包设置为8K的大小，第一次读取出来最大是8K
	_input.ensureFree(READ_WRITE_SIZE);

	//ret表示已经读取到的数据
	int ret = _socket->read(_input.getFree(), _input.getFreeLen());

	int read_cnt = 0;
	bool broken = false;

	//最大能连续读取10次，读任务便切换出来，给其他socket使用
	while (ret > 0 && ++read_cnt <= 10)
	{
		_input.pourData(ret);
		int decode = _streamer->decode(_input.getData(), _input.getDataLen(), &_input_queue);

		//一定要调用drainData，因为decode时候没有将_input的读位置前移。
		if (decode > 0)
		{
			_input.drainData(decode);
		}

		//如果发生了断开事件，或是_input没有读满（说明缓冲区里面已经没有数据了）
		if (broken || _input.getFreeLen() > 0) break;

		//如果判定读出来的包还没有解析完全说明，可能有未读出来的半包。
		//原先的判断条件为decode > 0, 修改为decode >= 0, decode == 0时可能是一个大包
		//数据包还没有完全接收完毕
		if (decode >= 0 && decode < _input.getDataLen())
		{
			_input.ensureFree(READ_WRITE_SIZE);
		}

		//todo: 如果发送是一个大包，要在encode体现出来，告知上层，那么_input继续扩大自己的范围来适应大包的发送。
		ret = _socket->read(_input.getFree(), _input.getFreeLen());
	}

	//对读到的数据业务回调处理，注意这个地方并不负责packet的释放，而是由外部来释放的
	if (_input_queue.size() > 0)
	{
		if (_server_adapter->_batch_push_packet)
		{
			_server_adapter->handle_batch_packet(this, _input_queue);
		}
		else
		{
			Packet *pack = NULL;
			while ((pack = _input_queue.pop()) != NULL)
			{
				_server_adapter->syn_handle_packet(this, pack);
			}
		}
	}

	//将读缓存区回归到初始位置
	_input.shrink();

	/*************
	 * broken事件在最后处理，且是在事件在外层调用的，所以当读取几个包后，读到断开事件时并不影响已经读到的数据的处理。但需要一个前提条件：
	 * （1）断开事件不能采用直接回调的方式，而是跟其它packet数据一样进行处理排队，这个地方要对packet进行处理
	 *************/
	if (!broken)
	{
		if (ret == 0)
		{
			OUT_INFO(NULL, 0, NULL, "%s recv 0, disconnect", _socket->get_addr().c_str());
			broken = true;
		}
		else if (ret < 0)
		{
			int error = Socket::get_last_error();
			broken = (error != EAGAIN);
		}
	}

	return !broken;
}

//postPacket作为客户端，主动发送数据的接口，client可以不用等到conn success回调成功，就调用这个接口。
//todo: 需要注意_outputQueue的实现中已经有锁了，外层又加了锁，后面要将这两个锁进行合并2014-10-11
bool TCPComponent::post_packet(Packet *packet)
{
	if (!is_conn_state())
	{
		//如果处于离线状态，而且不是自动重连的，那么直接返回
		if (is_auto_conn() == false)
		{
			return false;
		}
		//离线状态的最大缓存的包的数据不能超过10
		else if (_output_queue.size() > 10)
		{
			return false;
		}
		else
		{
			//init内部有驱动连接的过程；
			bool ret = init();
			if (!ret) return false;
		}
	}

// 如果是client, 并且有queue长度的限制
//	_output_mutex.lock();
//	_queueTotalSize = _output_queue.size() + _my_queue.size();
//	if (!_is_server && _queueLimit > 0 && _queueTotalSize >= _queueLimit)
//	{
//		_output_mutex.unlock();
//		return false;
//	}
//	_output_mutex.unlock();

	_output_mutex.lock();
	// 写入到outputqueue中
	_output_queue.push(packet);
	if (_output_queue.size() == 1U)
	{
		enable_write(true);
	}
	_output_mutex.unlock();

//	if (_is_server)
//	{
//		sub_ref();
//	}

	return true;
}

string TCPComponent::info()
{
	char buffer[512] = { 0 };
	snprintf(buffer, sizeof(buffer) - 1, "id:%lu type:%d state:%d used:%d fd:%d addr:%s peer:%s "
			"start_conn_time %lu last_use_time %lu", getid(), get_type(), get_state(), is_used(),
	        _socket->get_fd(), _socket->get_addr().c_str(), _socket->get_peer_addr().c_str(),
	        _start_conn_time, get_last_use_time());

	return buffer;
}

} /* namespace triones */
