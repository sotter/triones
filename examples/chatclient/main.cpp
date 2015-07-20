/*
 * main.cpp
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#include <stdio.h>
#include "chatclient.h"
#include "../chatxml/chatxml.h"
#include "comm/comlog.h"

using namespace triones;

void test()
{
	ChatXml chatxml;
	std::string xml = ChatXml::build_login_xml("vpn", "xb8s#FKD");
	printf("login:\n%s", xml.c_str());
	Command* cmd = ChatXml::parse_command(xml);
	if (cmd != NULL)
	{
		printf("cmd: %s\n", cmd->_cmd.c_str());
		Login *login = dynamic_cast<Login*>(cmd);
		delete login;
	}

	xml = ChatXml::build_logout_xml("vpn");
	printf("logout:\n%s", xml.c_str());
	cmd = ChatXml::parse_command(xml);
	if (cmd != NULL)
	{
		printf("cmd: %s\n", cmd->_cmd.c_str());
		Logout *logout = dynamic_cast<Logout*>(cmd);
		delete logout;
	}

	std::list<std::string> tousers;
	tousers.push_back("zhangsan");
	tousers.push_back("lisi");
	tousers.push_back("wangwu");
	tousers.push_back("zhaoliu");
	xml = ChatXml::build_send_xml("How are you?", "me", tousers);
	printf("send:\n%s", xml.c_str());
	cmd = ChatXml::parse_command(xml);
	if (cmd != NULL)
	{
		printf("cmd: %s\n", cmd->_cmd.c_str());
		SendMsg *sendmsg = dynamic_cast<SendMsg*>(cmd);
		delete sendmsg;
	}

	xml = ChatXml::build_groupsend_xml("How are you?", "me");
	printf("groupsend:\n%s", xml.c_str());
	cmd = ChatXml::parse_command(xml);
	if (cmd != NULL)
	{
		printf("cmd: %s\n", cmd->_cmd.c_str());
		GroupSendMsg *groupsendmsg = dynamic_cast<GroupSendMsg*>(cmd);
		delete groupsendmsg;
	}

	xml = ChatXml::build_receive_xml("How are you?", "zhangsan");
	printf("receive:\n%s", xml.c_str());
	cmd = ChatXml::parse_command(xml);
	if (cmd != NULL)
	{
		printf("cmd: %s\n", cmd->_cmd.c_str());
		Receive *receive = dynamic_cast<Receive*>(cmd);
		delete receive;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("usage: %s <name> <pwd>\n", argv[0]);
		return -1;
	}

	// 用户名、密码
	std::string name = argv[1];
	std::string pwd = argv[2];

	CHGLOG("test.log");
	SETLOGLEVEL(3);

	ChatClient chat_client;
	bool ret = chat_client.start("tcp:127.0.0.1:9494", 1);
	if (!ret)
	{
		printf("chat_client start failed\n");
		return -1;
	}

	// 1秒钟连接时间
	sleep(1);

	// 登录
	chat_client.to_login(name, pwd);

	std::string cmd;
	std::string param;
	for (;;)
	{
		printf("input cmd param\n");
		std::cin >> cmd;
		std::cin >> param;
		if (cmd.empty() || param.empty())
		{
			continue;
		}

		if (cmd == "send")
		{
			std::list<std::string> tousers = chat_client.split(param, ",");
			if (tousers.size() == 0)
			{
				continue;
			}

			chat_client.to_send_msg(name, tousers);
		}
		else if (cmd == "groupsend")
		{
			chat_client.to_groupsend(name);
		}
		else if (cmd == "logout")
		{
			chat_client.to_logout(name);
		}
		else if (cmd == "quit")
		{
			break;
		}
	}

	// 注销
	chat_client.stop_heartbeat();
	chat_client.to_logout(name);
	chat_client.destroy();

	OUT_INFO(NULL, 0, NULL, "good luck");

	LOGSTOP();
	return 0;
}

