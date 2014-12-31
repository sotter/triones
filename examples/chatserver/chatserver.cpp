/*
 * chatserver.cpp
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#include "chatserver.h"
#include "../chatxml/chatxml.h"
#include "user.h"

namespace triones
{

ChatServer::ChatServer()
{
	// TODO Auto-generated constructor stub
	_tp = NULL;
}

ChatServer::~ChatServer()
{
	// TODO Auto-generated destructor stub
}

bool ChatServer::start(const char *host, int thread)
{
	IOComponent *ioc = this->listen(host, triones::TPROTOCOL_TEXT);
	if (ioc == NULL)
	{
		printf("listen error \n");
		return false;
	}
	_tp = ioc->get_trans_proto();
	_id_user.set_trans_proto(_tp);
	_transport->add_timer_work(&_id_user);

	init(thread);
	return true;
}

// 处理包
void ChatServer::handle_packet(IOComponent *ioc, Packet *packet)
{
	if (ioc == NULL || packet == NULL)
	{
		return;
	}

	if (packet->get_type() == IServerAdapter::CMD_DISCONN_PACKET)
	{
		std::string name = ioc->get_userid();
		if (name.empty())
		{
			return;
		}

		size_t index = _id_user.get_hash_index(name);
		_id_user.ct_write_lock(index);
		_id_user.remove_user(index, name);
		_id_user.ct_write_unlock(index);
		return;
	}

	std::string xml(packet->getData(), packet->getDataLen());

	Command *cmd = ChatXml::parse_command(xml);
	if (cmd == NULL)
	{
		return;
	}

	Response resp;
	std::string command = cmd->_cmd;
	if (command != "heartbeat")
	{
		printf("<request_xml-------------------------------------------------------\n");
		printf("%s\n", xml.c_str());
		printf("-------------------------------------------------------request_xml>\n");
	}

	if (command == "login")
	{
		handle_user_login(ioc, dynamic_cast<Login*>(cmd), &resp);
	}
	else if (command == "logout")
	{
		handle_user_logout(ioc, dynamic_cast<Logout*>(cmd), &resp);
	}
	else if (command == "send")
	{
		handle_user_send_msg(ioc, dynamic_cast<SendMsg*>(cmd), &resp);
	}
	else if (command == "groupsend")
	{
		handle_user_group_send_msg(ioc, dynamic_cast<GroupSendMsg*>(cmd), &resp);
	}
	else if (command == "heartbeat")
	{
		resp._result = "succ";
	}
	else
	{
		delete cmd;
		return;
	}

	// 返回通用应答
	std::string resp_xml = ChatXml::build_response_xml(cmd->_cmd, resp._result);
	if (command != "heartbeat")
	{
		printf("<response_xml-------------------------------------------------------\n");
		printf("%s\n", resp_xml.c_str());
		printf("-------------------------------------------------------response_xml>\n");
	}

	Packet *pack = _tp->encode_pack(resp_xml.c_str(), resp_xml.length());
	if (pack)
	{
		if (!ioc->post_packet(pack))
		{
			delete pack;
		}
	}

	// 释放空间
	delete cmd;
}

// 处理用户登录
void ChatServer::handle_user_login(IOComponent *ioc, Login* login, Response *resp)
{
	size_t index = _id_user.get_hash_index(login->_name);

	BaseUser *user = NULL;
	_id_user.ct_read_lock(index);
	user = _id_user.get_user(index, login->_name);
	_id_user.ct_read_unlock(index);

	if (user != NULL)
	{
		resp->_result = login->_name + " has logined in";
		return;
	}

	user = new User();
	user->set_user_id(login->_name);
	user->set_ioc(ioc);

	bool ret = false;
	_id_user.ct_write_lock(index);
	ret = _id_user.add_user(index, user);
	if (ret)
	{
		ioc->set_userid(login->_name);
	}
	_id_user.ct_write_unlock(index);

	if (ret)
	{
		resp->_result = "succ";
	}
	else
	{
		resp->_result = "fail";
	}
}

// 处理用户注销
void ChatServer::handle_user_logout(IOComponent* ioc, Logout *logout, Response* resp)
{
	std::string name = ioc->get_userid();
	if (name.empty())
	{
		resp->_result = logout->_name + " has not logined in";
		return;
	}

	size_t index = _id_user.get_hash_index(name);

	BaseUser *user = NULL;

	_id_user.ct_read_lock(index);
	user = _id_user.get_user(index, name);
	_id_user.ct_read_unlock(index);

	if (NULL == user)
	{
		resp->_result = name + " has not logined in";
		return;
	}

	bool ret = false;
	_id_user.ct_write_lock(index);
	ret = _id_user.remove_user(index, name);
	IOComponent* bind_ioc = user->get_ioc();
	if (bind_ioc)
	{
		bind_ioc->set_userid("");
	}
	_id_user.ct_write_unlock(index);

	if (ret)
	{
		resp->_result = "succ";
	}
	else
	{
		resp->_result = "fail";
	}
}

// 处理用户发送消息
void ChatServer::handle_user_send_msg(IOComponent* ioc, SendMsg *sendmsg, Response* resp)
{
	std::string name = ioc->get_userid();
	if (name.empty())
	{
		resp->_result = sendmsg->_from_name + " has not logined in";
		return;
	}

	std::string xml = ChatXml::build_receive_xml(sendmsg->_msg, sendmsg->_from_name);

	std::list<std::string>& tousers = sendmsg->_to_list;
	std::list<std::string>::iterator it;
	for (it = tousers.begin(); it != tousers.end(); ++it)
	{
		size_t index = _id_user.get_hash_index(*it);

		BaseUser* user = NULL;
		IOComponent* ioc = NULL;

		bool send_succ = false;
		UNUSED(send_succ);

		_id_user.ct_read_lock(index);
		user = _id_user.get_user(index, *it);
		if (user)
		{
			ioc = user->get_ioc();
			if (ioc)
			{
				Packet* pack = new Packet();
				pack->writeBytes(xml.c_str(), xml.length());
				if (!ioc->post_packet(pack))
				{
					//printf("+++++++++++send to %s %s\n", it->c_str(), (send_succ ? "succ" : "false"));
					delete pack;
				}
				else
				{
					send_succ = true;
				}
			}
		}
		_id_user.ct_read_unlock(index);

		//printf(">>>>>>>>>>>send to %s %s\n", it->c_str(), (send_succ ? "succ" : "false"));
	}

	resp->_result = "succ";
}

// 处理用户群发消息
void ChatServer::handle_user_group_send_msg(IOComponent* ioc, GroupSendMsg *groupsendmsg,
        Response* resp)
{
	std::string name = ioc->get_userid();
	if (name.empty())
	{
		resp->_result = groupsendmsg->_from_name + " has not logined in";
		return;
	}

	std::string xml = ChatXml::build_receive_xml(groupsendmsg->_msg, groupsendmsg->_from_name);

	bool ret = _id_user.group_send(xml, name);

	if (ret)
	{
		resp->_result = "succ";
	}
	else
	{
		resp->_result = "fail";
	}
}

// 获取封包器
TransProtocol* ChatServer::get_trans_proto()
{
	return _tp;
}
} /* namespace triones */
