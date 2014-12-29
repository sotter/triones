/*
 * chatclient.h
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#ifndef CHATCLIENT_H_
#define CHATCLIENT_H_
#include "net/cnet.h"
#include "pack/tprotocol.h"
#include "comm/mutex.h"
#include "../chatxml/chatxml.h"
#include <list>

namespace triones {

class ChatClient : public BaseService, public triones::TBRunnable{
public:
	ChatClient();
	virtual ~ChatClient();

public:
	// 启动
	bool start(const char* host, int thread = 1);

	// 包处理函数
	virtual void handle_packet(IOComponent *ioc, Packet *packet);

	// 登录
	bool to_login(const std::string& name, const std::string& pwd);

	// 注销
	bool to_logout(const std::string& name);

	// 发消息
	bool to_send_msg(const std::string& from_name, const std::list<std::string>& tousers);

	// 群发消息
	bool to_groupsend(const std::string& from_name);

	// 拆分字符串
	std::list<std::string> split(const std::string& source, const std::string& spliter);

	// 停止线程
	void stop_heartbeat();
private:
	// 处理接收操作
	void handle_receive(Receive *receive);

	// 处理应答操作
	void handle_response(Response *response);

	// 线程处理函数
	virtual void run(TBThread *thread, void *arg);
private:
	// 连接
	IOComponent *_conn;

	// 封包器
	TransProtocol* _tp;

	// 打印锁
	triones::Mutex _print_mutex;

	// 心跳线程
	triones::TBThread _heartbeat_thread;
	bool _running;
};

} /* namespace triones */

#endif /* CHATCLIENT_H_ */
