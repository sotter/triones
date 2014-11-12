/******************************************************
 *   FileName: IOComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "tbtimeutil.h"

namespace triones
{

IOComponent::IOComponent(triones::Transport *owner, Socket *socket, int type, uint64_t id)
{
	assert(socket);
	_owner = owner;
	_socket = socket;
	_socket->set_ioc(this);
	_id = id;
	_sock_event = NULL;
	atomic_set(&_refcount, 0);
	_state = TRIONES_UNCONNECTED;
	_type = type;
	_auto_reconn = false;
	_pre = _next = NULL;
	_last_use_time = triones::CTimeUtil::get_time();
	_inused = false;

//	_is_server = false;
}

//socket放到子类中根据具体情况去释放。
//所有由UDPAcceptor派生出来的UDPComponent公用一个socket,所以不能在这里释放。
IOComponent::~IOComponent()
{

}

//由子类实现，非必须实现的接口
bool IOComponent::post_packet(Packet *packet)
{
	UNUSED(packet);
	return false;
}

void IOComponent::enable_write(bool on)
{
	if (_sock_event)
	{
		_sock_event->set_event(_socket, true, on);
	}
}

} /* namespace triones */
