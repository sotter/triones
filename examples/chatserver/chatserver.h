/*
 * chatserver.h
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#ifndef CHATSERVER_H_
#define CHATSERVER_H_
#include "net/cnet.h"
#include "pack/tprotocol.h"
#include "../chatxml/chatxml.h"
#include "user/baseuser.h"
#include "user/idbaseuser.h"
#include "iduser.h"

namespace triones {

class ChatServer : public BaseService {
public:
	ChatServer();
	virtual ~ChatServer();

	// 启动服务器
	bool start(const char *host, int thread = 1);

	// 处理包
	virtual void handle_packet(IOComponent *ioc, Packet *packet);

	// 获取封包器
	TransProtocol* get_trans_proto();

private:
	// 处理用户登录
	void handle_user_login(IOComponent *ioc, Login* login, Response *resp);

	// 处理用户注销
	void handle_user_logout(IOComponent* ioc, Logout *logout, Response* resp);

	// 处理用户发送消息
	void handle_user_send_msg(IOComponent* ioc, SendMsg *sendmsg, Response* resp);

	// 处理用户群发消息
	void handle_user_group_send_msg(IOComponent* ioc, GroupSendMsg *groupsendmsg, Response* resp);
private:
	TransProtocol *_tp;
	IdUser _id_user;
};

} /* namespace triones */

#endif /* CHATSERVER_H_ */
