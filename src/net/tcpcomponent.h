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
	        IServerAdapter *serverAdapter, int type = 0);

	virtual ~TCPComponent();

	bool init(bool isServer = false);

	void close();

	// socket异常事件的处理函数EPOLLERR| EPOLLHUP
	// virtual bool handleExpEvent();

	// socket写事件处理函数 EPOLLOUT
	virtual bool handle_write_event();

	// socket读事件处理函数 EOPLLIN
	virtual bool handle_read_event();

	// TransPort定时器定时回调处理的函数
	bool check_timeout(uint64_t now);

	// 异步连接处理
	bool socket_connect();

	void disconnect();

	// 设置是否为服务器端
	void setServer(bool is_server)
	{
		_is_server = is_server;
	}

	//postPacket作为客户端，主动发送数据的接口，client可以不用等到conn success回调成功，就调用这个接口。
	bool post_packet(Packet *packet);

	//readData后直接调用的handlePakcet
//	bool handle_packet(Packet *packet);

	//写事件，调用的发送函数
	virtual bool write_data();

	//从socket中读取数据
	virtual bool read_data();

	void set_write_finish_close(bool v)
	{
		_write_finish_close = v;
	}

	//  清空output的buffer
	void clear_output_buffer()
	{
		_output.clear();
	}

	// clear input buffer
	void clear_input_buffer()
	{
		_input.clear();
	}

private:
	// TCP连接
	uint64_t _start_conn_time;

	/**   原先connection的部分  ****************/
	//这里的_isServer指的accpect出来的socket，而不是listen socket
	bool _is_server;                         // 是服务器端
	Socket *_socket;                        // Socket句柄
	TransProtocol *_streamer;               // Packet解析

	PacketQueue _output_queue;               // 发送队列
	PacketQueue _input_queue;                // 接收队列
	PacketQueue _my_queue;                   // 在write中处理时暂时用
	triones::Mutex _output_mutex;           // 发送队列锁

//	int _queueTimeout;                      // 队列超时时间
//	int _queueTotalSize;                    // 队列总长度
//	int _queueLimit;                        // 队列最长长度, 如果超过这个值post进来就会被wait

	/**  TCPCONNECTION 部分  ******************/
	DataBuffer _output;      // 输出的buffer
	DataBuffer _input;       // 读入的buffer
	bool _write_finish_close;  // 写完断开, 供短连接业务使用的
};

} /* namespace triones */
#endif /* TCPCOMPONENT_H_ */
