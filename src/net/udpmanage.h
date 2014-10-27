/******************************************************
 *   FileName: udpmanage.h
 *     Author: triones  2014-10-23
 *Description:
 *******************************************************/

#ifndef UDPMANAGE_H_
#define UDPMANAGE_H_

#include "tqueue.h"

namespace triones
{

class UDPManage
{
public:
	UDPManage(){}
	virtual ~UDPManage(){}

	UDPComponent *get(uint64_t sockid, Socket *socket, bool connected = true)
	{
		UDPComponent *ioc = NULL;

		_mutex.lock();
		std::map<uint64_t, UDPComponent *>::iterator iter = _mpsock.find(sockid);

		if(iter != _mpsock.end())
		{
			ioc = iter->second;
			_mutex.unlock();
			return ioc;
		}

		ioc = (UDPComponent*)_queue.pop() ;
		if ( ioc == NULL ) {
			ioc = new UDPComponent;
		}


		ioc->_type = FD_UDP ;
		ioc->init( sockfd, ip, port , ctype ) ;
		ioc->_last = time(NULL) ;  // 最后一次使用时间
		ioc->_ptr  = NULL ; // 第三方扩展数据
		ioc->_next = NULL ;
		ioc->_pre  = NULL ;
		ioc->_activity = ( connected ) ? SOCKET_CONNECTED: SOCKET_CONNECTING; // 是否已连接成功

		// 针对服务器的FD资源单独自己管理
		if ( queue ) {
			_online.push( p ) ;
			_index.insert( std::set<socket_t*>::value_type(p) ) ;
			_mpsock.insert( std::make_pair( szid, p ) ) ;
		}
		_mutex.unlock() ;
	}

	void put(UDPComponent *ioc)
	{

	}

private:
	// 数据队列头
	TQueue<UDPComponent> _queue ;
	// 在线队列查找索引
	std::set<UDPComponent*> _index ;
	// 在线队列管理
	TQueue<UDPComponent> _online ;
	// 数据同步操作锁
	triones::Mutex _mutex ;
	// 连接对象查找管理
	std::map<uint64_t, UDPComponent*> _mpsock;
};

} /* namespace triones */
#endif /* UDPMANAGE_H_ */
