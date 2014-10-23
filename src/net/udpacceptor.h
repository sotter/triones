/******************************************************
 *   FileName: UDPAcceptor.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef UDPACCEPTOR_H_
#define UDPACCEPTOR_H_

//UDP包的最大长度即为64KB
#define TRIONES_UDP_RECV_SIZE (64 * 1024)

namespace triones
{
class UDPAcceptor : public IOComponent
{
public:
    /**
    * 构造函数，由Transport调用。
    *
    * @param  owner:    运输层对象
    * @param  socket:   Socket对象
    * @param streamer:   数据包的双向流，用packet创建，解包，组包。
    * @param serverAdapter:  用在服务器端，当Connection初始化及Channel创建时回调时用
    */
    UDPAcceptor(Transport *owner, Socket *socket,
    		triones::TransProtocol *streamer, IServerAdapter *serverAdapter);

    virtual ~UDPAcceptor(){}
    /*
     * 初始化
     *
     * @return 是否成功
     */
    bool init(bool isServer = false);

    /**
    * 当有数据可读时被Transport调用
    *
    * @return 是否成功, true - 成功, false - 失败。
    */
    bool handleReadEvent();

    /**
     * 在accept中没有写事件
     */
    bool handleWriteEvent()
    {
        return true;
    }

    bool readData();

    bool writeData();

    void checkTimeout(int64_t now);

private:

    int     _udp_type;
    struct  sockaddr_in      _sock_addr;

	//这里的_isServer指的accpect出来的socket，而不是listen socket
    bool _isServer;                         // 是服务器端
    Socket *_socket;                        // Socket句柄
    TransProtocol *_streamer;               // Packet解析

    triones::Mutex _output_mutex;           // 发送队列锁

    char _read_buff[TRIONES_UDP_RECV_SIZE]; //UDP接收长度

    bool _gotHeader;                        // packet header已经取过
    bool _writeFinishClose;                 // 写完断开
};

} /* namespace triones */

#endif /* UDPACCEPTOR_H_ */
