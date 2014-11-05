/******************************************************
 *   FileName: TCPComponent.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef TCPCOMPONENT_H_
#define TCPCOMPONENT_H_

#include "../pack/tprotocol.h"
#include "../comm/mutex.h"
#include "../comm/databuffer.h"

#define READ_WRITE_SIZE 8192
#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

namespace triones
{

class TCPComponent: public IOComponent
{
public:
	TCPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
	        IServerAdapter *serverAdapter);

	virtual ~TCPComponent();

	bool init(bool isServer = false);

	void close();

	// socket异常事件的处理函数EPOLLERR| EPOLLHUP
	// virtual bool handleExpEvent();

	// socket写事件处理函数 EPOLLOUT
	virtual bool handleWriteEvent();

	// socket读事件处理函数 EOPLLIN
	virtual bool handleReadEvent();

	// TransPort定时器定时回调处理的函数
	void checkTimeout(int64_t now);

	// 异步连接处理
	bool socket_connect();

	void disconnect();

	// 设置是否为服务器端
	void setServer(bool isServer)
	{
		_isServer = isServer;
	}

	//postPacket作为客户端，主动发送数据的接口，client可以不用等到conn success回调成功，就调用这个接口。
	bool postPacket(Packet *packet);

	//readData后直接调用的handlePakcet
	bool handlePacket(Packet *packet);

	//写事件，调用的发送函数
	virtual bool writeData();

	//从socket中读取数据
	virtual bool readData();

	//设置对列的超时时间
	void setQueueTimeout(int queueTimeout)
	{
		_queueTimeout = queueTimeout;
	}

	// 设置queue最大长度, 0 - 不限制
	void setQueueLimit(int limit)
	{
		_queueLimit = limit;
	}

	uint64_t getServerId()
	{
		if (_socket)
		{
			return _socket->getId();
		}
		return 0;
	}

	uint64_t getPeerId()
	{
		if (_socket)
		{
			return _socket->getPeerId();
		}
		return 0;
	}

	int getLocalPort()
	{
		if (_socket)
		{
			return _socket->getLocalPort();
		}
		return -1;
	}

	void setWriteFinishClose(bool v)
	{
		_writeFinishClose = v;
	}

	//  清空output的buffer
	void clearOutputBuffer()
	{
		_output.clear();
	}

	// clear input buffer
	void clearInputBuffer()
	{
		_input.clear();
	}

private:
	// TCP连接
	time_t _startConnectTime;

	/**   原先connection的部分  ****************/
	//这里的_isServer指的accpect出来的socket，而不是listen socket
	bool _isServer;                         // 是服务器端
	Socket *_socket;                        // Socket句柄
	TransProtocol *_streamer;               // Packet解析

	PacketQueue _outputQueue;               // 发送队列
	PacketQueue _inputQueue;                // 接收队列
	PacketQueue _myQueue;                   // 在write中处理时暂时用
	triones::Mutex _output_mutex;           // 发送队列锁

	int _queueTimeout;                      // 队列超时时间
	int _queueTotalSize;                    // 队列总长度
	int _queueLimit;                        // 队列最长长度, 如果超过这个值post进来就会被wait

	/**  TCPCONNECTION 部分  ******************/
	DataBuffer _output;      // 输出的buffer
	DataBuffer _input;       // 读入的buffer
	bool _writeFinishClose;  // 写完断开, 供短连接业务使用的
};

} /* namespace triones */
#endif /* TCPCOMPONENT_H_ */
