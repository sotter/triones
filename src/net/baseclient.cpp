/******************************************************
 *   FileName: baseclient.cpp
 *     Author: triones  2015-1-13
 *Description:
 *******************************************************/

#include "cnet.h"
#include "../comm/comlog.h"
//#include "userjober.h"

namespace triones
{

BaseClient::BaseClient(int stream, IUserFactory* user_factory, int user_type, int thread_num /*= 4*/)
: _stream(stream)
, _user_factory(user_factory)
, _user_type(user_type)
, _app_thread_num(thread_num)
{

}

BaseClient::~BaseClient()
{

}

//取得配置，如udp:192.168.100.48:7008;
bool BaseClient::init_param()
{
	//留给子类来实现
	return true;
}

//启动client，所有的配置都只能从配置获取不再支持直接传值的方式
bool BaseClient::start()
{
	if (!init_param()) {
		return false;
	}

	if (!init(_app_thread_num)) {
		return false;
	}

	//第一次到来时全部遍历连接一遍
	JoberHolder holder;
	holder.set(this, &BaseClient::user_scan_connect);
	_online_user.traverse_all_user(holder, true);

	return true;
}

// 停止client
void BaseClient::stop()
{
	// 销毁底层网络库
	destroy();
}


//初始化连接列表，每一个连接列表对应着一个user，提供给子类使用的方法类
BaseUser* BaseClient::add_conn(const std::string& user_id, const string& host, unsigned short port, bool tcp = true, bool auto_reconn = true)
{
	if (host.empty() || port <= 0) {
		return NULL;
	}

	char buffer[64] = {0};
	snprintf("%s:%s:%d", sizeof(buffer) - 1, tcp ? "tcp" : "udp", host.c_str(), port);

	// 创建连接
	IOComponent* ioc = connect(buffer, _stream, auto_reconn);
	if (!ioc) {
		return NULL;
	}
	ioc->set_userid(user_id);

	// 创建用户
	BaseUser *user = _user_factory->create_user(_user_type);
	if (!user)
	{
		if (ioc->_auto_reconn) {
			// 自动重连ioc，必须显式关闭它,否则ioc将永远不会被释放
			ioc->close();
		}

		return NULL;
	}

	/*
	 * 不能用buffer做为user_id，否则：连接同一server的user具有相同的user_id
	 */
	user->set_user_id(user_id);
	user->set_conn_addr(buffer);
	user->bind_ioc(ioc);

	if( !_online_user.add_user(user))
	{
		if (ioc->_auto_reconn) {
			// 自动重连ioc，必须显式关闭它,否则ioc将永远不会被释放
			ioc->close();
		}

		user->unbind_ioc();
		delete user;
		return NULL;
	}

	return user;
}

// 添加用户
bool BaseClient::add_user(BaseUser* user)
{
	if (!user || user->_user_id.empty()) {
		return false;
	}

	if( !_online_user.add_user(user)) {
		return false;
	}

	return true;
}

//支持初始连接列表的方式，如 tcp:127.0.0.1:7008,udp:127.0.0.1:7008,
bool BaseClient::add_conn_list(const string conn_list)
{
	//2015-01-14，暂时先不实现
	UNUSED(conn_list);
	return true;
}

//连接成功的回调处理
bool BaseClient::handle_connected(IOComponent *connection, bool succ)
{
	//如果没有连接成功，下面的业务暂时不做；
	if (!succ) return false;

	string user_id = connection->get_userid();
	size_t index = _online_user.get_hash_index(user_id);

	RWGuard(_online_user.get_lock(index), true);

	BaseUser* user = (BaseUser*) _online_user.get_user(index, user_id);
	if (user == NULL)
	{
		OUT_INFO(NULL, 0, NULL, "connect %s cannot get the user",
		        connection->get_socket()->get_addr().c_str());
		return false;
	}

	Packet *login = new Packet;
	if (cons_login_packet(user, login))
	{
		if (connection->post_packet(login))
		{
			return true;
		}
	}

	delete login;

	return false;
}

//定时器的回调处理，timer是由底层Transport的定时线程回调上来的，不能有过大的阻塞的操作；
void BaseClient::handle_timer_work(uint64_t now)
{
	/*
	 * 默认操作是定时发送心跳数据包
	 */
	JoberHolder holder;
	holder.set(this, &BaseClient::user_scan_noop);
	_online_user.traverse_all_user(holder, false);
}

//遍历用户的回调函数
//void BaseClient::user_scan_handler(BaseUser *user, uint64_t now, void *param)
//{
//	// 默认操作是定时发送心跳数据包
//	user_scan_noop(now, user);
////	long type = (long)param;
////
////	if (type == SCAN_TYPE_CONN)
////	{
////		user_scan_connect(now, user);
////	}
////	else if (type == SCAN_TYPE_NOOP)
////	{
////		user_scan_noop(now, user);
////	}
//}

void BaseClient::user_scan_connect(uint64_t now, BaseUser *user)
{
	// 如果网络底层库会自动重连，这里不做重连操作
	if (user->get_ioc()->is_auto_conn()) {
		return;
	}

	// 目录NOT处于离线状态,不需要重连
	if (user->_user_state != USER_OFF_LINE) {
		return;
	}

	if (user->_ioc != NULL)
	{
		//base_user跟_ioc进行解绑
		user->unbind_ioc();
	}

	//重连时间间隔暂时不可配,注意connect失败时，重连问题；
	IOComponent *ioc = connect(user->_conn_addr.c_str(), _stream, true);

	if (ioc != NULL)
	{
		user->bind_ioc(ioc);
	}

	return;
}

//遍历发送心跳包
void BaseClient::user_scan_noop(uint64_t now, BaseUser *user)
{
	if (user->_user_state == USER_ONLINE)
	{
		// 网络底层库在空闲连接时间大于30秒时，将关闭连接，
		// 考虑到加解锁、及发送延时，这里保守一点，采用每20秒内最少传输有一个数据包的机制
		if (now - user->_last_active_time < 20) {
			user->_last_active_time = now;
			return;
		}

		Packet *noop = new Packet;
		if (cons_noop_packet(user, noop))
		{
			if (user->_ioc->post_packet(noop))
			{
				return;
			}
		}
		delete noop;
	}
}

// 通知收到数据包
bool BaseClient::handle_packet(IOComponent *connection, Packet *packet)
{
	BaseUser* user = NULL;
	std::string user_id = connection->get_userid();
	size_t index = _online_user.get_hash_index(connection->get_userid());

	// 保证用户正在处理期间不会被释放掉
	UserHolder holder;
	{
		RWGuard guard(_online_user._lock_array[index], false);
		user = _online_user.get_user(index, user_id);
		if (!user || user->get_ioc() != connection) {
			return false;
		}

		holder.set_user(user);
	}

	bool ret = false;
	if (packet->get_type() == IServerAdapter::CMD_DISCONN_PACKET) {
		ret = handle_disconnected(user);
	}
	else if (packet->get_type == IServerAdapter::CMD_DATA_PACKET) {
		ret = handle_user_packet(user, packet);
	}

	return ret;
}

//断开连接
bool BaseClient::handle_disconnected(BaseUser* user)
{
	size_t index = user->get_hash_index();

	RWGuard guard(_online_user._lock_array[index], true);
	user->set_state(triones::USER_OFF_LINE);
	return true;
}
} /* namespace triones */
