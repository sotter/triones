/******************************************************
 *   FileName: baseserver.cpp
 *     Author: triones  2015-1-13
 *Description:
 *******************************************************/

#include "cnet.h"

namespace triones
{
	/*
	 * 构造函数
	 */
	BaseServer::BaseServer(int stream, IUserFactory* user_factory, int user_type, int thread_num = 4)
	: _stream(stream)
	, _user_factory(user_factory)
	, _user_type(user_type)
	, _app_thread_num(thread_num)
	{
	}

	/*
	 * 虚析构函数
	 */
	BaseServer::~BaseServer(){}

	/*
	 * 功能：启动BaseServer
	 * 返回值：true-启动成功；false-启动失败
	 */
	bool BaseServer::start_base_server(const char* host)		// ip地址，如tcp:127.0.0.1:8080
	{
		// 开始监听端口
		IOComponent* ioc = this->listen(host, _stream);
		if (NULL == ioc) {
			return false;
		}

		// 启动event_loop线程和工作线程
		bool succ = false;
		succ = this->init(_app_thread_num);
		if (!succ) {
			return false;
		}

		return true;
	}

	/*
	 * 功能：停止server
	 * 返回值：无
	 *
	 */
	void BaseServer::stop_base_server()
	{
		this->destroy();
	}

	/*
	 * 通知收到数据包
	 * 这个函数把断连数据包和普通数据包分开处理了
	 */
	bool BaseServer::handle_packet(IOComponent *connection, Packet *packet)
	{
		BaseUser* user = NULL;
		std::string user_id = connection->get_userid();
		size_t index = _online_user.get_hash_index(connection->get_userid());

		// 保证用户正在处理期间不会被释放掉
		UserHolder holder;
		{
			RWGuard guard(_online_user._lock_array[index], false);
			user = _online_user.get_user(index, user_id);
			if (user && user->get_ioc() == connection) {
				// 更新时间
				user->_last_active_time = time(NULL);

				holder.set_user(user);
			} else {
				// 可能是登录数据包，继承流程就OK
			}
		}

		/*
		 * 处理数据包
		 * 注：如果是登录数据包，user可能为NULL
		 */
		bool ret = false;
		if (packet->get_type() == IServerAdapter::CMD_DISCONN_PACKET) {
			ret = handle_disconnected(user);
		} else if (packet->get_type == IServerAdapter::CMD_DATA_PACKET) {
			ret = handle_user_packet(user, packet);
		}

		return ret;
	}

	/*
	 * 功能：处理断开连接数据包
	 * 描述：本操作仅仅将User的状态置为USER_OFF_LINE，然后online_user列表会在循环过程中
	 * 		被释放掉
	 */
	bool BaseServer::handle_disconnected(BaseUser* user)
	{
		size_t index = user->get_hash_index();

		RWGuard guard(_online_user._lock_array[index], true);
		user->set_state(triones::USER_OFF_LINE);
		return true;
	}


	//定时器的回调处理，timer是由底层Transport的定时线程回调上来的，不能有过大的阻塞的操作；
	void BaseServer::handle_timer_work(uint64_t now)
	{
		// 定期删除过期用户
		_online_user.delete_user(now);
	}

	void BaseServer::user_scan_connect(uint64_t now, BaseUser *user)
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

} /* namespace triones */
