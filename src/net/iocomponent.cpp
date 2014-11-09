/******************************************************
 *   FileName: IOComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "tbtimeutil.h"

namespace triones
{

IOComponent::IOComponent(triones::Transport *owner, Socket *socket, int type)
{
	assert(socket);
	_owner = owner;
	_socket = socket;
	_socket->setIOComponent(this);
	_socketEvent = NULL;
	atomic_set(&_refcount, 0);
	_state = TRIONES_UNCONNECTED; // 正在连接
	_type = type;
	_autoReconn = false; // 不要自动重连
	_pre = _next = NULL;
	_lastUseTime = triones::CTimeUtil::getTime();
	_inUsed = false;
	_isServer = false;
}

//socket放到子类中根据具体情况去释放。
//所有由UDPAcceptor派生出来的UDPComponent公用一个socket,所以不能在这里释放。
IOComponent::~IOComponent()
{
}

triones::Transport *IOComponent::getOwner()
{
	return _owner;
}

} /* namespace triones */
