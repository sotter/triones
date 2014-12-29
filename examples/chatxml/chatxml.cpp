/*
 * chatxml.cpp
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#include "chatxml.h"
#include "../../src/tinyxml/tinyxml.h"
#include "../../src/tinyxml/tinystr.h"

namespace triones {

ChatXml::ChatXml() {
	// TODO Auto-generated constructor stub

}

ChatXml::~ChatXml() {
	// TODO Auto-generated destructor stub
}

// 生成登录xml
std::string ChatXml::build_login_xml(const std::string& name, const std::string& pwd)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "login");
	doc.LinkEndChild(chat);

	// 添加user
	TiXmlElement *user = new TiXmlElement("user");
	user->SetAttribute("name", name.c_str());
	user->SetAttribute("pwd", pwd.c_str());
	chat->LinkEndChild(user);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();

	return xml;
}

// 生成注销xml
std::string ChatXml::build_logout_xml(const std::string& name)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "logout");
	doc.LinkEndChild(chat);

	// 添加user
	TiXmlElement *user = new TiXmlElement("user");
	user->SetAttribute("name", name.c_str());
	chat->LinkEndChild(user);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 生成发送消息xml
std::string ChatXml::build_send_xml(const std::string& msg, const std::string& from_name, const std::list<std::string>& tousers)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "send");
	doc.LinkEndChild(chat);

	// 添加to
	TiXmlElement *to = new TiXmlElement("to");
	to->SetAttribute("from_name", from_name.c_str());
	to->SetAttribute("msg", msg.c_str());
	chat->LinkEndChild(to);

	// 添加user
	for (std::list<std::string>::const_iterator it = tousers.begin(); it != tousers.end(); ++it) {
		TiXmlElement *user = new TiXmlElement("user");
		user->SetAttribute("name", it->c_str());
		to->LinkEndChild(user);
	}

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 生成群发消息xml
std::string ChatXml::build_groupsend_xml(const std::string& msg, const std::string& from_name)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "groupsend");
	doc.LinkEndChild(chat);

	// 添加to
	TiXmlElement *to = new TiXmlElement("to");
	to->SetAttribute("from_name", from_name.c_str());
	to->SetAttribute("msg", msg.c_str());
	chat->LinkEndChild(to);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 生成接收消息xml
std::string ChatXml::build_receive_xml(const std::string& msg, const std::string& from_user)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "receive");
	doc.LinkEndChild(chat);

	// 添加from
	TiXmlElement *from = new TiXmlElement("from");
	from->SetAttribute("from_name", from_user.c_str());
	from->SetAttribute("msg", msg.c_str());
	chat->LinkEndChild(from);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 生成应答消息
std::string ChatXml::build_response_xml(const std::string& request, const std::string& result)
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "response");
	doc.LinkEndChild(chat);

	// 添加response
	TiXmlElement *resp = new TiXmlElement("response");
	resp->SetAttribute("request", request.c_str());
	resp->SetAttribute("result", result.c_str());
	chat->LinkEndChild(resp);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 生成心跳请求
std::string ChatXml::build_heartbeat_xml()
{
	std::string xml = "";

	// 不用delete任何元素，doc会保证释放内存
	TiXmlDocument doc;

	// 添加描述
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	// 添加chat
	TiXmlElement *chat = new TiXmlElement("chat");
	chat->SetAttribute("command", "heartbeat");
	doc.LinkEndChild(chat);

	// 获取xml
	TiXmlPrinter printer;
	doc.Accept(&printer);
	xml = printer.CStr();
	return xml;
}

// 解析消息体
Command* ChatXml::parse_command(const std::string& xml)
{
	TiXmlDocument doc;
	doc.Parse(xml.c_str());

	TiXmlElement *chat = doc.RootElement();
	if (chat == NULL) {
		return NULL;
	}

	// 获取命令字
	std::string cmd = chat->Attribute("command");

	// 解析协议
	if (cmd == "login") {
		// 获取user
		TiXmlElement *user = chat->FirstChildElement("user");
		if (user == NULL) {
			return NULL;
		}
		std::string name = user->Attribute("name");
		std::string pwd = user->Attribute("pwd");

		// 构造Login实例
		Login *login = new Login();
		login->_cmd = cmd;
		login->_name = name;
		login->_pwd = pwd;
		return login;
	} else if (cmd == "logout") {
		// 获取user
		TiXmlElement *user = chat->FirstChildElement("user");
		if (user == NULL) {
			return NULL;
		}
		std::string name = user->Attribute("name");

		// 构造Login实例
		Logout *logout = new Logout();
		logout->_cmd = cmd;
		logout->_name = name;
		return logout;
	} else if (cmd == "send") {
		// 获取to
		TiXmlElement *to = chat->FirstChildElement("to");
		if (to == NULL) {
			return NULL;
		}

		SendMsg *sendmsg = new SendMsg();

		// 获取消息
		std::string msg = to->Attribute("msg");
		std::string from_name = to->Attribute("from_name");
		sendmsg->_cmd = cmd;
		sendmsg->_msg = msg;
		sendmsg->_from_name = from_name;

		// 获取用户
		for (TiXmlElement *user = to->FirstChildElement("user"); user != NULL; user = user->NextSiblingElement("user")) {
			std::string name = user->Attribute("name");
			sendmsg->_to_list.push_back(name);
		}

		return sendmsg;
	} else if (cmd == "groupsend") {
		// 获取to
		TiXmlElement *to = chat->FirstChildElement("to");
		if (to == NULL) {
			return NULL;
		}

		GroupSendMsg *groupsendmsg = new GroupSendMsg();

		// 获取消息
		std::string msg = to->Attribute("msg");
		std::string from_name = to->Attribute("from_name");
		groupsendmsg->_cmd = cmd;
		groupsendmsg->_msg = msg;
		groupsendmsg->_from_name = from_name;

		return groupsendmsg;
	} else if (cmd == "receive") {
		// 获取from
		TiXmlElement *from = chat->FirstChildElement("from");
		if (from == NULL) {
			return NULL;
		}

		std::string from_name = from->Attribute("from_name");
		std::string msg = from->Attribute("msg");

		Receive *receive = new Receive();
		receive->_cmd = cmd;
		receive->_from_name = from_name;
		receive->_msg = msg;

		return receive;
	} else if (cmd == "response") {
		// 获取response
		TiXmlElement *response = chat->FirstChildElement("response");
		if (NULL == response) {
			return NULL;
		}

		std::string request = response->Attribute("request");
		std::string result = response->Attribute("result");

		Response *resp = new Response();
		resp->_cmd = cmd;
		resp->_request = request;
		resp->_result = result;

		return resp;
	} else if (cmd == "heartbeat") {
		HeartBeat* heart_beat = new HeartBeat();
		heart_beat->_cmd = "heartbeat";
		return heart_beat;
	}
	return NULL;
}

} /* namespace triones */
