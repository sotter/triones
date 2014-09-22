/******************************************************
 *   FileName: TCPComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "tcpcomponent.h"
#include "tbtimeutil.h"
#include "../../comlog.h"
#include "databuffer.h"
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
	_defaultPacketHandler = NULL;
	_iocomponent = NULL;
	_queueTimeout = 5000;
	_queueLimit = 50;
	_queueTotalSize = 0;

	/**tcpconnection ****/
	_gotHeader = false;
	_writeFinishClose = false;
	memset(&_packetHeader, 0, sizeof(_packetHeader));
}

//析构函数
TCPComponent::~TCPComponent()
{
	//将CONNECTION和TCPCONNECTION的析构函数放到这里
	//tcpconnection 无内容
	//connection内容
	disconnect();
	_socket = NULL;
	_iocomponent = NULL;
}

//连接断开，降所有发送队列中的packet全部超时
void TCPComponent::disconnect()
{
	_output_mutex.lock();
	_myQueue.moveTo(&_outputQueue);
	_output_mutex.unlock();
	checkTimeout(TBNET_MAX_TIME);
}

//连接到指定的机器, isServer: 是否初始化一个服务器的Connection
bool TCPComponent::init(bool isServer)
{
	_socket->setSoBlocking(false);
	_socket->setSoLinger(false, 0);
	_socket->setReuseAddress(true);
	_socket->setIntOption(SO_KEEPALIVE, 1);
	_socket->setIntOption(SO_SNDBUF, 64000);
	_socket->setIntOption(SO_RCVBUF, 64000);
	_socket->setTcpNoDelay(true);

	if (!isServer)
	{
		if (!socket_connect() && _autoReconn == false)
		{
			return false;
		}
	}
	else
	{
		_state = TRIONES_CONNECTED;
	}

	setServer(isServer);
	_isServer = isServer;

	return true;
}

/*
 * 连接到socket
 */
bool TCPComponent::socket_connect()
{
	if (_state == TRIONES_CONNECTED || _state == TRIONES_CONNECTING)
	{
		return true;
	}
	_socket->setSoBlocking(false);

	if (_socket->connect())
	{
		if (_socketEvent)
		{
			_socketEvent->addEvent(_socket, true, true);
		}
		_state = TRIONES_CONNECTED;

		_startConnectTime = time(NULL);
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
			OUT_ERROR(NULL, 0, NULL, "连接到 %s 失败, %s(%d)", _socket->getAddr().c_str(),
			        strerror(error), error);
			return false;
		}
	}
	return true;
}

void TCPComponent::close()
{
	if (_socket)
	{
		if (_socketEvent)
		{
			_socketEvent->removeEvent(_socket);
		}
		if (_connection && isConnectState())
		{
			//将这个事件转化成一个socket转发出去
			setDisconnState();
		}
		_socket->close();

		if (_connection)
		{
			clearInputBuffer(); // clear input buffer after socket closed
		}

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
 *
 * @param    now 当前时间(单位us)
 */
void TCPComponent::checkTimeout(int64_t now)
{
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
	// 超时检查
	checkTimeout(now);
}

/**** 原先connectiong的部分 *********************/
void TCPComponent::disconnect()
{
	_output_mutex.lock();
	_myQueue.moveTo(&_outputQueue);
	_output_mutex.unlock();
	checkTimeout(TBNET_MAX_TIME);
}

/*
 * 发送packet到发送队列
 */
bool TCPComponent::postPacket(Packet *packet, IPacketHandler *packetHandler, void *args,
        bool noblocking)
{
	if (!isConnectState())
	{
		if (_iocomponent == NULL || _iocomponent->isAutoReconn() == false)
		{
			return false;
		}
		else if (_outputQueue.size() > 10)
		{
			return false;
		}
		else
		{
			TCPComponent *ioc = dynamic_cast<TCPComponent*>(_iocomponent);
			bool ret = false;
			if (ioc != NULL)
			{
				_output_mutex.lock();
				ret = ioc->init(false);
				_output_mutex.unlock();
			}
			if (!ret) return false;
		}
	}

//	// 如果是client, 并且有queue长度的限制
//	_output_mutex.lock();
//	_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
//	if (!_isServer && _queueLimit > 0 && noblocking && _queueTotalSize >= _queueLimit)
//	{
//		_output_mutex.unlock();
//		return false;
//	}
//	_output_mutex.unlock();
//	Channel *channel = NULL;
//	packet->setExpireTime(_queueTimeout);           // 设置超时
//	if (_streamer->existPacketHeader())
//	{           // 存在包头
//		uint32_t chid = packet->getChannelId();     // 从packet中取
//		if (_isServer)
//		{
//			assert(chid != 0);                      // 不能为空
//		}
//		else
//		{
//			channel = _channelPool.allocChannel();
//
//			// channel没找到了
//			if (channel == NULL)
//			{
//				OUT_ASSERT(NULL, 0, NULL, "分配channel出错, id: %u", chid);
//				return false;
//			}
//
//			channel->setHandler(packetHandler);
//			channel->setArgs(args);
//			packet->setChannel(channel);            // 设置回去
//		}
//	}
//	_output_mutex.lock();
//	// 写入到outputqueue中
//	_outputQueue.push(packet);
//	if (_iocomponent != NULL && _outputQueue.size() == 1U)
//	{
//		_iocomponent->enableWrite(true);
//	}
//	_output_mutex.unlock();
//	if (!_isServer && _queueLimit > 0)
//	{
//		_output_mutex.lock();
//		_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
//		if (_queueTotalSize > _queueLimit && noblocking == false)
//		{
//			bool *stop = NULL;
//			if (_iocomponent && _iocomponent->getOwner())
//			{
//				stop = _iocomponent->getOwner()->getStop();
//			}
//			while (_queueTotalSize > _queueLimit && stop && *stop == false)
//			{
//				if (_outputCond.wait(1000) == false)
//				{
//					if (!isConnectState())
//					{
//						break;
//					}
//					_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount()
//					        + _myQueue.size();
//				}
//			}
//		}
//		_output_mutex.unlock();
//	}

	if (_isServer && _iocomponent)
	{
		_iocomponent->subRef();
	}

	return true;
}

/*
 * handlePacket 数据
 */
bool TCPComponent::handlePacket(DataBuffer *input, PacketHeader *header)
{
	//客户端的发送没有确认方式，所有client和server都是采用handlerpacket的方式
	if (_iocomponent)
		_iocomponent->addRef();

	Packet *packet;
	return _serverAdapter->handlePacket(this, packet);

//	Packet *packet;
//	IPacketHandler::HPRetCode rc;
//	void *args = NULL;
//	Channel *channel = NULL;
//	IPacketHandler *packetHandler = NULL;
//
//	if (_streamer->existPacketHeader() && !_isServer)
//	{ // 存在包头
//		uint32_t chid = header->_chid;    // 从header中取
//		chid = (chid & 0xFFFFFFF);
//		channel = _channelPool.offerChannel(chid);
//
//		// channel没找到
//		if (channel == NULL)
//		{
//			input->drainData(header->_dataLen);
//			OUT_ASSERT(NULL, 0, NULL, "没找到channel, id: %u, %s", chid,
//			        tbsys::CNetUtil::addrToString(getServerId()).c_str());
//			return false;
//		}
//
//		packetHandler = channel->getHandler();
//		args = channel->getArgs();
//	}
//
//	// 解码
//	packet = _streamer->decode(input, header);
//	if (packet == NULL)
//	{
//		packet = &ControlPacket::BadPacket;
//	}
//	else
//	{
//		packet->setPacketHeader(header);
//		// 是批量调用, 直接放入queue, 返回
//		if (_isServer && _serverAdapter->_batchPushPacket)
//		{
//			if (_iocomponent) _iocomponent->addRef();
//			_inputQueue.push(packet);
//			if (_inputQueue.size() >= 15)
//			{ // 大于15个packet就调用一次
//				_serverAdapter->handleBatchPacket(this, _inputQueue);
//				_inputQueue.clear();
//			}
//			return true;
//		}
//	}
//
//	// 调用handler
//	if (_isServer)
//	{
//		if (_iocomponent) _iocomponent->addRef();
//		rc = _serverAdapter->handlePacket(this, packet);
//	}
//	else
//	{
//		if (packetHandler == NULL)
//		{    // 用默认的
//			packetHandler = _defaultPacketHandler;
//		}
//		assert(packetHandler != NULL);
//
//		rc = packetHandler->handlePacket(packet, args);
//		channel->setArgs(NULL);
//		// 接收回来释放掉
//		if (channel)
//		{
//			_channelPool.appendChannel(channel);
//		}
//	}
//
//	return true;
}

///*
// * 客户端的连接超时检测
// */
//bool TCPComponent::checkTimeout(int64_t now)
//{
//	// 得到超时的channel的list
//	Channel *list = _channelPool.getTimeoutList(now);
//	Channel *channel = NULL;
//	IPacketHandler *packetHandler = NULL;
//
//	if (list != NULL)
//	{
//		if (!_isServer)
//		{ // client endpoint, 给每个channel发一个超时packet, 服务器端把channel回收
//			channel = list;
//			while (channel != NULL)
//			{
//				packetHandler = channel->getHandler();
//				if (packetHandler == NULL)
//				{    // 用默认的
//					packetHandler = _defaultPacketHandler;
//				}
//				// 回调
//				if (packetHandler != NULL)
//				{
//					packetHandler->handlePacket(&ControlPacket::TimeoutPacket, channel->getArgs());
//					channel->setArgs(NULL);
//				}
//				channel = channel->getNext();
//			}
//		}
//		// 加到freelist中
//		_channelPool.appendFreeList(list);
//	}
//
//	// 对PacketQueue超时检查
//	_output_mutex.lock();
//	Packet *packetList = _outputQueue.getTimeoutList(now);
//	_output_mutex.unlock();
//	while (packetList)
//	{
//		Packet *packet = packetList;
//		packetList = packetList->getNext();
//		channel = packet->getChannel();
//		packet->free();
//		if (channel)
//		{
//			packetHandler = channel->getHandler();
//			if (packetHandler == NULL)
//			{    // 用默认的
//				packetHandler = _defaultPacketHandler;
//			}
//			// 回调
//			if (packetHandler != NULL)
//			{
//				packetHandler->handlePacket(&ControlPacket::TimeoutPacket, channel->getArgs());
//				channel->setArgs(NULL);
//			}
//			_channelPool.freeChannel(channel);
//		}
//	}
//
//	// 如果是client, 并且有queue长度的限制
//	if (!_isServer && _queueLimit > 0 && _queueTotalSize > _queueLimit)
//	{
//		_output_mutex.lock();
//		_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
//		if (_queueTotalSize <= _queueLimit)
//		{
//			_outputCond.broadcast();
//		}
//		_output_mutex.unlock();
//	}
//
//	return true;
//}

/**
 * 连接状态
 */
bool TCPComponent::isConnectState()
{
	if (_iocomponent != NULL)
	{
		return _iocomponent->isConnectState();
	}
	return false;
}

/********* 原先TCPCONNECTION的地方  **************************/
/** 说明 2014-09-21 刘波  **
 * （1）这个地方应该限定写入次数，如果在单线程的条件下，写入的数据量过大，导致其它的socket的任务一直处于等待状态 ，
 * 在这里面默认最大是只写10次的。
 * （2）对于写事件中出现的异常，如EPIPE等，没有作特殊处理，这样的话只能通过读事件来判定socket的断开情况。
 * （3）将packet序列化为流数据的方式。在淘宝的框架中，所有的通信内部都是采用同一种协议，
 * 用同一种协议进行了封装，数据传输都是通过packet的方式。这个packet是带有chanelID的，这样发送端和接收端的业务就能对应起来。
 * 而在我们的业务中，实现的是跟通用客户端的通信，这种客户端可能不cnet写的，而是走的协议（transprotocol）
 *  *********/

bool TCPComponent::writeData()
{
	// 把 _outputQueue copy到 _myQueue中
	_output_mutex.lock();
	_outputQueue.moveTo(&_myQueue);

	//如果socket中的数据已经全部发送完毕了，置可写事件为false，然后退出来
	if (_myQueue.size() == 0 && _output.getDataLen() == 0)
	{
		_iocomponent->enableWrite(false);
		_output_mutex.unlock();
		return true;
	}
	_output_mutex.unlock();

	Packet *packet;
	int ret;
	int writeCnt = 0;
	int myQueueSize = _myQueue.size();

	do
	{
		// 写满到
		while (_output.getDataLen() < READ_WRITE_SIZE)
		{
			// 队列空了就退出
			if (myQueueSize == 0)
				break;

			packet = _myQueue.pop();
			myQueueSize--;
			_streamer->encode(packet, &_output);
//			_channelPool.setExpireTime(packet->getChannel(), packet->getExpireTime());
			packet->free();
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
	if ((queueSize == 0 || _writeFinishClose)
			&& _iocomponent != NULL)
	{
		_iocomponent->enableWrite(false);
	}
	_output_mutex.unlock();

	if (_writeFinishClose)
	{
		OUT_ERROR(NULL, 0, NULL, "主动断开.");
		return false;
	}

//	// 如果是client, 并且有queue长度的限制
//	if (!_isServer && _queueLimit > 0 && _queueTotalSize > _queueLimit)
//	{
//		_output_mutex.lock();
//		_queueTotalSize = queueSize + _channelPool.getUseListCount();
//		if (_queueTotalSize <= _queueLimit)
//		{
//			_outputCond.broadcast();
//		}
//		_output_mutex.unlock();
//	}

	return true;


	printf("GGIIITTT\n");

}

/****************************
 *(1)read
 ***************************/
bool TCPComponent::readData()
{
	_input.ensureFree(READ_WRITE_SIZE);
	int ret = _socket->read(_input.getFree(), _input.getFreeLen());
	int readCnt = 0;
	int freeLen = 0;
	bool broken = false;

	while (ret > 0)
	{
		_input.pourData(ret);
		freeLen = _input.getFreeLen();

		while (1)
		{
			if (!_gotHeader)
			{
				_gotHeader = _streamer->getPacketInfo(&_input, &_packetHeader, &broken);
				if (broken) break;
			}
			// 如果有足够的数据, decode, 并且调用handlepacket
			if (_gotHeader && _input.getDataLen() >= _packetHeader._dataLen)
			{
				handlePacket(&_input, &_packetHeader);
				_gotHeader = false;
				_packetHeader._dataLen = 0;
				TBNET_COUNT_PACKET_READ(1);
			}
			else
			{
				break;
			}
		}

		if (broken || freeLen > 0 || readCnt >= 10)
		{
			break;
		}

		if (_packetHeader._dataLen - _input.getDataLen() > READ_WRITE_SIZE)
		{
			_input.ensureFree(_packetHeader._dataLen - _input.getDataLen());
		}
		else
		{
			_input.ensureFree(READ_WRITE_SIZE);
		}

		ret = _socket->read(_input.getFree(), _input.getFreeLen());
		readCnt++;
	}

	_socket->setTcpQuickAck(true);

	// 是否为批量回调
	if (_isServer && _serverAdapter->_batchPushPacket && _inputQueue.size() > 0)
	{
		_serverAdapter->handleBatchPacket(this, _inputQueue);
		_inputQueue.clear();
	}

	_input.shrink();
	if (!broken)
	{
		if (ret == 0)
		{
			broken = true;
		}
		else if (ret < 0)
		{
			int error = Socket::getLastError();
			broken = (error != EAGAIN);
		}
	}
	else
	{
		_gotHeader = false;
	}

	return !broken;
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
