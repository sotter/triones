/******************************************************
 *   FileName: IOComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "iocomponent.h"

namespace triones
{
IOComponent::IOComponent(triones::Transport *owner, Socket *socket)
{
    assert(socket);
    _owner = owner;
    _socket = socket;
    _socket->setIOComponent(this);
    _socketEvent = NULL;
    atomic_set(&_refcount, 0);
    _state = TRIONES_UNCONNECTED; // 正在连接
    _autoReconn = false; // 不要自动重连
    _prev = _next = NULL;
    _lastUseTime = triones::CTimeUtil::getTime();
    _inUsed = false;
}

/*
 * 析构函数
 */
IOComponent::~IOComponent()
{
    if (_socket) {
        _socket->close();
        delete _socket;
        _socket = NULL;
    }
}

/**
 * owner
 */
triones::Transport *IOComponent::getOwner()
{
    return _owner;
}

} /* namespace triones */
