/******************************************************
 *   FileName: TCPAcceptor.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_

#include "../pack/tprotocol.h"

namespace triones
{

class TCPAcceptor : public IOComponent
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
    TCPAcceptor(Transport *owner, Socket *socket,
    		triones::TransProtocol *streamer, IServerAdapter *serverAdapter);

    virtual ~TCPAcceptor(){}
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
    bool handleWriteEvent() {
        return true;
    }

    /*
     * 超时检查
     *
     * @param    now 当前时间(单位us)
     */
    void checkTimeout(int64_t now);

private:
    TransProtocol *_streamer;      // 数据包解析器
//    IServerAdapter  *_serverAdapter; // 服务器适配器
};

} /* namespace triones */
#endif /* TCPACCEPTOR_H_ */
