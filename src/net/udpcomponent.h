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

class UDPComponent : public IOComponent
{
public:

	friend class UDPManage;
	friend class UDPAcceptor;

    UDPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
    		IServerAdapter *serverAdapter, int type = 0);

    ~UDPComponent();

    //初始化
    bool init(bool isServer = false);

    //关闭
    void close();

    //TransPort可写的回调函数
    bool handleWriteEvent();

    //TransPort可读的回调函数
    bool handleReadEvent();

    bool readData();

    bool writeData();

	bool postPacket(Packet *packet);

    //检查超时，检查_online并回调超时处理函数
    void checkTimeout(int64_t now);

private:
    int     _udp_type;

    struct  sockaddr_in      _sock_addr;

	//这里的_isServer指的accpect出来的socket，而不是listen socket
    bool _isServer;                         // 是服务器端
    TransProtocol *_streamer;               // Packet解析

    PacketQueue _outputQueue;               // 发送队列
    PacketQueue _inputQueue;                // 接收队列
    PacketQueue _myQueue;                   // 在write中处理时暂时用

    //UDP接收长度
    char _read_buff[TRIONES_UDP_RECV_SIZE];
};

} /* namespace triones */
#endif /* UDPCOMPONENT_H_ */
