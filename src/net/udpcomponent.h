/******************************************************
 *   FileName: UDPComponent.h
 *     Author: triones  2014-9-18
 *Description: 对于UDP来说，对于服务端都是由UDPAcceptor来处理，所有的发送上来的客户端都使用一个socket。
 *Description: 对于客户端来说，每一个客户端为其分配一个socket
 *******************************************************/

#ifndef UDPCOMPONENT_H_
#define UDPCOMPONENT_H_

namespace triones
{

class UDPComponent: public IOComponent
{
public:

	friend class UDPManage;
	friend class UDPAcceptor;

	UDPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
	        IServerAdapter *serverAdapter, int type = 0);

	~UDPComponent();

	//初始化
	bool init();

	//关闭
	void close();

	//TransPort可写的回调函数
	virtual bool handle_write_event();

	//TransPort可读的回调函数
	virtual bool handle_read_event();

//	virtual std::string info();

	bool read_data();

	bool write_data();

	bool post_packet(Packet *packet);

	//检查超时，检查_online并回调超时处理函数
	bool check_timeout(uint64_t now);

	// udp连接
	bool socket_connect();

	void set_addr(struct sockaddr_in *addr)
	{
		if (addr) {
			_sock_addr = *addr;
		}
	}

private:
	uint64_t _start_conn_time;				// 开始连接时间
	struct sockaddr_in _sock_addr;

	//这里的_isServer指的accpect出来的socket，而不是listen socket
	bool _isServer;                         // 是服务器端

	PacketQueue _outputQueue;               // 发送队列
	PacketQueue _inputQueue;                // 接收队列
	PacketQueue _myQueue;                   // 在write中处理时暂时用

	//UDP接收长度
	char _read_buff[TRIONES_UDP_RECV_SIZE];
};

} /* namespace triones */
#endif /* UDPCOMPONENT_H_ */
