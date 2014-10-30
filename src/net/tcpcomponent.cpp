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

#define TBNET_MAX_TIME (1ll<<62)

namespace triones
{

TCPComponent::TCPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter)
		: IOComponent(owner, socket)
{
	_startConnectTime = 0;
	_isServer = false;

	/**connection ****/
	_socket = socket;
	_streamer = streamer;
	_serverAdapter = serverAdapter;
//	_defaultPacketHandler = NULL;
//	_iocomponent = NULL;
	_queueTimeout = 5000;
	_queueLimit = 50;
	_queueTotalSize = 0;

	/**tcpconnection ****/
	_gotHeader = false;
	_writeFinishClose = false;
//	memset(&_packetHeader, 0, sizeof(_packetHeader));
}

//析构函数
TCPComponent::~TCPComponent()
{
	__INTO_FUN__
	//将CONNECTION和TCPCONNECTION的析构函数放到这里
	//tcpconnection 无内容
	//connection内容
	disconnect();
	_socket = NULL;
//	_iocomponent = NULL;
}

//连接断开，降所有发送队列中的packet全部超时
void TCPComponent::disconnect()
{
	__INTO_FUN__
	_output_mutex.lock();
	_myQueue.moveto(&_outputQueue);
	_output_mutex.unlock();
	checkTimeout(TBNET_MAX_TIME);
}

//连接到指定的机器, isServer: 是否初始化一个服务器的Connection
bool TCPComponent::init(bool isServer)
{
	__INTO_FUN__
	_socket->setSoBlocking(false);
	_socket->setSoLinger(false, 0);
	_socket->setReuseAddress(true);
	_socket->setIntOption(SO_KEEPALIVE, 1);
	_socket->setIntOption(SO_SNDBUF, 64000);
	_socket->setIntOption(SO_RCVBUF, 64000);
	_socket->setTcpNoDelay(true);

	if (!isServer)
	{
		printf("%s %d \n", __FILE__, __LINE__);
		if (!socket_connect() && _autoReconn == false)
		{
			printf("%s %d \n", __FILE__, __LINE__);
			return false;
		}
	}
	else
	{
		printf("%s %d \n", __FILE__, __LINE__);
		_state = TRIONES_CONNECTED;
	}

	printf("%s %d \n", __FILE__, __LINE__);
	setServer(isServer);
	_isServer = isServer;

	return true;
}


/*
 * 连接到socket
 */
bool TCPComponent::socket_connect()
{
	__INTO_FUN__

	//注意，这个函数必须是可重入的，可能有不同用户的线程调用这个接口，完全依靠state判断也不是很严密的；2014-10-11
	if (_state == TRIONES_CONNECTED || _state == TRIONES_CONNECTING)
	{
		return true;
	}
	_socket->setSoBlocking(false);

	_startConnectTime = time(NULL);
	if (_socket->connect())
	{
		if (_socketEvent)
		{
			_socketEvent->addEvent(_socket, true, true);
		}
		_state = TRIONES_CONNECTED;
	}
	else
	{
		int error = Socket::getLastError();
		if (error == EINPROGRESS || error == EWOULDBLOCK)
		{
			_state = TRIONES_CONNECTING;

			if (_socketEvent)
			{
				_socketEvent->addEvent(_socket, true, true);
			}
		}
		else
		{
			_socket->close();
			_state = TRIONES_CLOSED;
			OUT_ERROR(NULL, 0, NULL, "connect %s fail, %s(%d)", _socket->getAddr().c_str(),
			        strerror(error), error);
			return false;
		}
	}
	return true;
}

void TCPComponent::close()
{
	__INTO_FUN__
	if (_socket)
	{
		if (_socketEvent)
		{
			_socketEvent->removeEvent(_socket);
		}
		if (isConnectState())
		{
			//将这个事件转化成一个socket转发出去
			setDisconnState();
		}
		_socket->close();

		clearInputBuffer(); // clear input buffer after socket closed

		_state = TRIONES_CLOSED;
	}
}

/*
 * 当有数据可写到时被Transport调用
 *
 * @return 是否成功, true - 成功, false - 失败。
 */
bool TCPComponent::handleWriteEvent()
{
	__INTO_FUN__

	_lastUseTime = triones::CTimeUtil::getTime();
	bool rc = true;
	if (_state == TRIONES_CONNECTED)
	{
		rc = writeData();
	}
	else if (_state == TRIONES_CONNECTING)
	{
		int error = _socket->getSoError();
		if (error == 0)
		{
			enableWrite(true);
			clearOutputBuffer();
			_state = TRIONES_CONNECTED;
		}
		else
		{
			OUT_ERROR(NULL, 0, NULL, "连接到 %s 失败: %s(%d)", _socket->getAddr().c_str(),
			        strerror(error), error);
			if (_socketEvent)
			{
				_socketEvent->removeEvent(_socket);
			}
			_socket->close();
			_state = TRIONES_CLOSED;
		}
	}
	return rc;
}

/**
 * 当有数据可读时被Transport调用
 *
 * @return 是否成功, true - 成功, false - 失败。
 */
bool TCPComponent::handleReadEvent()
{
	__INTO_FUN__
	_lastUseTime = triones::CTimeUtil::getTime();
	bool rc = false;
	if (_state == TRIONES_CONNECTED)
	{
		rc = readData();
	}
	return rc;
}

/*
 * 超时检查
 * @param    now 当前时间(单位us)
 */
void TCPComponent::checkTimeout(int64_t now)
{
	//	__INTO_FUN__
	// 检查是否连接超时
	if (_state == TRIONES_CONNECTING)
	{
		if (_startConnectTime > 0 && _startConnectTime < (now - static_cast<int64_t>(2000000)))
		{ // 连接超时 2 秒
			_state = TRIONES_CLOSED;
			OUT_ERROR(NULL, 0, NULL, "连接到 %s 超时.", _socket->getAddr().c_str());
			_socket->shutdown();
		}
	}
	else if (_state == TRIONES_CONNECTED && _isServer == true && _autoReconn == false)
	{ // 连接的时候, 只用在服务器端
		int64_t idle = now - _lastUseTime;
		if (idle > static_cast<int64_t>(900000000))
		{ // 空闲15min断开
			_state = TRIONES_CLOSED;
			OUT_INFO(NULL, 0, NULL, "%s 空闲了: %d (s) 被断开.", _socket->getAddr().c_str(),
			        (idle / static_cast<int64_t>(1000000)));
			_socket->shutdown();
		}
	}
	//需要重连的socket
	else if(_state == TRIONES_CLOSED && _isServer == false && _autoReconn == true)
	{
		//每隔五秒钟重连一次
		if (_startConnectTime > 0 && _startConnectTime < (now - static_cast<int64_t>(5000000)))
		{
			//不管是否连接成功，都更新连接时间，时间间隔都是5000000
			_startConnectTime = time(NULL);
			socket_connect();
		}
	}

	// 原先connect的超时检查
    // checkTimeout(now);
}

/**** 原先connectiong的部分 *********************/

/*
 * handlePacket 数据
 */
bool TCPComponent::handlePacket(Packet *packet)
{
	__INTO_FUN__
	//客户端的发送没有确认方式，所有client和server都是采用handlerpacket的方式
	this->addRef();

	return _serverAdapter->SynHandlePacket(this, packet);
}

/*** 说明 2014-09-21
 * （1）这个地方应该限定写入次数，如果在单线程的条件下，写入的数据量过大，导致其它的socket的任务一直处于等待状态 ，
 * 在这里面默认最大是只写10次的。
 * （2）对于写事件中出现的异常，如EPIPE等，没有作特殊处理，这样的话只能通过读事件来判定socket的断开情况。
 * （3）将packet序列化为流数据的方式。在淘宝的框架中，所有的通信内部都是采用同一种协议，
 * 用同一种协议进行了封装，数据传输都是通过packet的方式。这个packet是带有chanelID的，这样发送端和接收端的业务就能对应起来。
 * 而在我们的业务中，实现的是跟通用客户端的通信，这种客户端可能不cnet写的，而是走的协议（transprotocol）
 *  *********/
bool TCPComponent::writeData()
{
	__INTO_FUN__
	// 把 _outputQueue copy到 _myQueue中, 从_myQueue中向外发送
	_output_mutex.lock();
	_outputQueue.moveto(&_myQueue);

	//如果socket中的数据已经全部发送完毕了，置可写事件为false，然后退出来
	if (_myQueue.size() == 0 && _output.getDataLen() == 0)
	{
		this->enableWrite(false);
		_output_mutex.unlock();
		return true;
	}
	_output_mutex.unlock();

	Packet *packet;
	int ret;
	int writeCnt = 0;
	int myQueueSize = _myQueue.size();

	//todo:这块代码需要重写，packet本身就是从_output继承过来的
	//如果write出现ERRORAGAIN的情况，下一个包继续发送；

	do
	{
		while (_output.getDataLen() < READ_WRITE_SIZE)
		{
			// 队列空了就退出
			if (myQueueSize == 0) break;
			packet = _myQueue.pop();

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
	} while (ret > 0 && _output.getDataLen() == 0 && myQueueSize > 0 && writeCnt < 10);

	// 紧缩
	_output.shrink();

	_output_mutex.lock();
	int queueSize = _outputQueue.size() + _myQueue.size() + (_output.getDataLen() > 0 ? 1 : 0);
	if (queueSize == 0 || _writeFinishClose)
	{
		this->enableWrite(false);
	}
	_output_mutex.unlock();

	if (_writeFinishClose)
	{
		OUT_ERROR(NULL, 0, NULL, "主动断开.");
		return false;
	}

	return true;
}

/****************************
 *(1)socket能够读取的最大的包是多少
 ***************************/
bool TCPComponent::readData()
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
		int decode = _streamer->decode(_input.getData(), _input.getDataLen(), &_inputQueue);

		//一定要调用drainData，因为decode时候没有将_input的读位置前移。
		if(decode > 0)
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
	if (_inputQueue._size > 0)
	{
		if (_serverAdapter->_batchPushPacket)
		{
			_serverAdapter->handleBatchPacket(this, _inputQueue);
		}
		else
		{
			Packet *pack = NULL;
			while ((pack = _inputQueue.pop()) != NULL)
			{
				_serverAdapter->SynHandlePacket(this, pack);
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
			OUT_INFO(NULL, 0, NULL, "%s recv 0, disconnect", _socket->getAddr().c_str());
			broken = true;
		}
		else if (ret < 0)
		{
			int error = Socket::getLastError();
			broken = (error != EAGAIN);
		}
	}

	return !broken;
}

//postPacket作为客户端，主动发送数据的接口，client可以不用等到conn success回调成功，就调用这个接口。
//todo: 需要注意_outputQueue的实现中已经有锁了，外层又加了锁，后面要将这两个锁进行合并2014-10-11
bool TCPComponent::postPacket(Packet *packet)
{
	if (!isConnectState())
	{
		//如果处于离线状态，而且不是自动重连的，那么直接返回
		if (isAutoReconn() == false)
		{
			return false;
		}
		//离线状态的最大缓存的包的数据不能超过10
		else if (_outputQueue.size() > 10)
		{
			return false;
		}
		else
		{
			//init内部有驱动连接的过程；
			bool ret = init(false);
			if (!ret) return false;
		}
	}

	// 如果是client, 并且有queue长度的限制
	_output_mutex.lock();
	_queueTotalSize = _outputQueue.size() + _myQueue.size();
	if (!_isServer && _queueLimit > 0 && _queueTotalSize >= _queueLimit)
	{
		_output_mutex.unlock();
		return false;
	}
	_output_mutex.unlock();

	_output_mutex.lock();
	// 写入到outputqueue中
	_outputQueue.push(packet);
	if (_outputQueue.size() == 1U)
	{
		enableWrite(true);
	}
	_output_mutex.unlock();

	if (_isServer)
	{
		subRef();
	}

	return true;
}

/**
 * 发送setDisconnState, 客户端的on_dis_connection
 */
void TCPComponent::setDisconnState()
{
	disconnect();
//	if (_defaultPacketHandler && _isServer == false)
//	{
//		_defaultPacketHandler->handlePacket(&ControlPacket::DisconnPacket, _socket);
//	}
}

} /* namespace triones */
