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

class TCPAcceptor: public IOComponent
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
	TCPAcceptor(Transport *owner, Socket *socket, triones::TransProtocol *streamer,
	        IServerAdapter *adapter);

	virtual ~TCPAcceptor();

	//初始化
	bool init();

	// 当有数据可读时被Transport调用
	virtual bool handle_read_event();

	//在accept中不处理写事件
	virtual bool handle_write_event()
	{
		return true;
	}

	//IOComponent 网络资源清理
	virtual void close();

	//超时检查
	virtual bool check_timeout(uint64_t now);

};

} /* namespace triones */
#endif /* TCPACCEPTOR_H_ */
