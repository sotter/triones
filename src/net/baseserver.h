/******************************************************
 *   FileName: baseserver.h
 *     Author: triones  2015-1-13
 *Description:
 *******************************************************/

#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "../comm/databuffer.h"
#include "baseuser.h"

namespace triones
{

/*
 * 功能：基础server，包括以下功能：
 * 		1)_online_user
 * 		2)支持客户端登录功能
 */
class BaseServer: public BaseService
{
public:
	/*
	 * 功能：构造函数
	 */
	BaseServer(int stream, IUserFactory* user_factory, int user_type, int thread_num = 4);

	/*
	 * 功能：虚析构函数
	 */
	~BaseServer();

	/*
	 * 功能：启动BaseServer
	 * 返回值：true-启动成功；false-启动失败
	 */
	virtual bool start_base_server(const char* host);		// ip地址，如tcp:127.0.0.1:8080

	/*
	 * 功能：停止server
	 * 返回值：无
	 */
	virtual void stop_base_server();

	/*
	 * 通知收到数据包
	 * 这个函数把断连数据包和普通数据包分开处理了
	 */
	virtual bool handle_packet(IOComponent *connection, 				// 底层ioc
										Packet *packet);				// 底层ioc收到的数据包

	/*
	 * 功能：断开连接的默认操作
	 * 描述：本操作仅仅将用户状态置为USER_OFF_LINE，然后等待被回收
	 */
	virtual bool handle_disconnected(BaseUser* user);					// 断开连接的用户

	/*
	 * 功能：处理数据
	 */
	virtual bool handle_user_packet(BaseUser* user,						// 数据包对应的用户
									Packet *packet) = 0;				// 用户对应的数据包

	/*
	 * 功能：定时器的回调处理
	 * 描述：timer是由底层Transport的定时线程回调上来的，不能有过大的阻塞的操作；
	 */
	virtual void handle_timer_work(uint64_t now);						// 当前时间

	/*
	 * 功能：遍历连接
	 */
	void user_scan_connect(uint64_t now,								// 当前时间
							BaseUser *user);							// online_user列表上的某一用户
protected:
	// 解包器类型
	int _stream;

	// 业务层的线程个数，默认是4个
	int _app_thread_num;

	// 抽象用户工厂
	IUserFactory* _user_factory;

	// 用户类型
	int _user_type;

	// 用户列表
	OnlineUser _online_user;
};

} /* namespace triones */
#endif /* BASESERVER_H_ */
