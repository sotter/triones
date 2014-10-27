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

private:
//    __gnu_cxx::hash_map<int, UDPConnection*> _connections;  // UDP连接集合
    IServerAdapter *_serverAdapter;

    int     _udp_type;
    struct  sockaddr_in      _sock_addr;

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
    bool _gotHeader;            // packet header已经取过
    bool _writeFinishClose;     // 写完断开
};

} /* namespace triones */
#endif /* UDPCOMPONENT_H_ */
