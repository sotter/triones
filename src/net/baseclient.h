/******************************************************
 *   FileName: baseclient.h
 *     Author: triones  2015-1-13
 *Description:
 *******************************************************/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "../comm/databuffer.h"
#include "baseuser.h"

namespace triones
{

class BaseClient: public BaseService//, public UserScanHandler
{
public:
	BaseClient(int stream, IUserFactory* user_factory, int user_type, int thread_num = 4);

	virtual ~BaseClient();

	bool init_param();

	//初始化连接列表
	BaseUser* add_conn(const std::string& user_id, const string& host, unsigned short port, bool tcp = true, bool auto_reconn = true);

	// 添加用户,user中必须包含user_id
	bool add_user(BaseUser* user);

	//支持初始连接列表的方式，如 tcp:127.0.0.1:7008,udp:127.0.0.1:7008,
	bool add_conn_list(const string conn_list);

	//启动client，所有的配置都只能从配置获取不再支持直接传值的方式
	virtual bool start();

	// 停止client
	virtual void stop();

	//够构造登录包
	virtual bool cons_login_packet(BaseUser* user, Packet *packet) = 0;

	//构造心跳包
	virtual bool cons_noop_packet(BaseUser* user, Packet *packet) = 0;

	//连接成功的回调处理
	virtual bool handle_connected(IOComponent *connection, bool succ);

	/*
	 * 通知收到数据包
	 * 这个函数把断连数据包和普通数据包分开处理了
	 */
	virtual bool handle_packet(IOComponent *connection, Packet *packet);

	//断开连接的默认操作
	virtual bool handle_disconnected(BaseUser* user);

	//处理数据
	virtual bool handle_user_packet(BaseUser* user, Packet *packet) = 0;

	//定时器的回调处理，timer是由底层Transport的定时线程回调上来的，不能有过大的阻塞的操作；
	virtual void handle_timer_work(uint64_t now);

//	enum
//	{
//		SCAN_TYPE_CONN = 0, SCAN_TYPE_NOOP, SCAN_TYPE_NUM
//	};

	//遍历用户的回调函数
	//virtual void user_scan_handler(BaseUser *user, uint64_t now, void *param);

	//遍历连接
	void user_scan_connect(uint64_t now, BaseUser *user);

	//遍历发送心跳包
	void user_scan_noop(uint64_t now, BaseUser *user);

private:
	// 解包器类型
	int _stream;

	//业务层的线程数，默认是4个
	int _app_thread_num;

	// 抽象用户工厂
	IUserFactory* _user_factory;

	// 用户列表
	OnlineUser _online_user;

	// 用户类型
	int _user_type;
};

} /* namespace triones */
#endif /* BASECLIENT_H_ */
