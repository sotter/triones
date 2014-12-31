/*
 * chatxml.h
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#ifndef CHATXML_H_
#define CHATXML_H_
#include <list>
#include <string>

namespace triones
{

class Command;
class ChatXml
{
public:
	ChatXml();
	virtual ~ChatXml();

	// 生成登录xml
	static std::string build_login_xml(const std::string& name, const std::string& pwd);

	// 生成注销xml
	static std::string build_logout_xml(const std::string& name);

	// 生成发送消息xml
	static std::string build_send_xml(const std::string& msg, const std::string& from_name,
	        const std::list<std::string>& tousers);

	// 生成群发消息xml
	static std::string build_groupsend_xml(const std::string& msg, const std::string& from_name);

	// 生成接收消息xml
	static std::string build_receive_xml(const std::string& msg, const std::string& from_user);

	// 生成应答消息
	static std::string build_response_xml(const std::string& request, const std::string& result);

	// 生成心跳请求
	static std::string build_heartbeat_xml();

	// 解析消息体
	static Command* parse_command(const std::string& xml);
};

class Command
{
public:
	virtual ~Command()
	{
	}
	std::string _cmd;
};

class Login: public Command
{
public:
	std::string _name;
	std::string _pwd;
};

class Logout: public Command
{
public:
	std::string _name;
};

class SendMsg: public Command
{
public:
	std::string _msg;
	std::string _from_name;
	std::list<std::string> _to_list;
};

class GroupSendMsg: public Command
{
public:
	std::string _msg;
	std::string _from_name;
};

class Receive: public Command
{
public:
	std::string _from_name;
	std::string _msg;
};

class Response: public Command
{
public:
	std::string _request;
	std::string _result;
};

class HeartBeat: public Command
{
public:
};

} /* namespace triones */

#endif /* CHATXML_H_ */
