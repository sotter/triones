/*
 * chatclient.cpp
 *
 *  Created on: 2014年12月17日
 *      Author: water
 */

#include "chatclient.h"

namespace triones {

ChatClient::ChatClient() {
	// TODO Auto-generated constructor stub
	_conn = NULL;
	_running = false;
	_tp = NULL;
}

ChatClient::~ChatClient() {
	// TODO Auto-generated destructor stub
}

// 启动
bool ChatClient::start(const char* host, int thread)
{
	bool ret = init(thread);
	if (!ret) {
		return false;
	}

	_conn = connect(host, triones::TPROTOCOL_TEXT, true);
	if (NULL == _conn)
	{
		return false;
	}
	_tp = _conn->get_trans_proto();
	_heartbeat_thread.start(this, NULL);
	_running = true;
	return true;
}

// 包处理函数
void ChatClient::handle_packet(IOComponent *ioc, Packet *packet)
{
	if (!ioc || !packet) {
		return;
	}

	std::string xml(packet->getData(), packet->getDataLen());
	Command *cmd = ChatXml::parse_command(xml);
	if (!cmd) {
		return;
	}

	if (cmd->_cmd == "response") {
		handle_response(dynamic_cast<Response*>(cmd));
	} else if (cmd->_cmd == "receive") {
		handle_receive(dynamic_cast<Receive*>(cmd));
	}
	delete cmd;
}

// 处理接收操作
void ChatClient::handle_receive(Receive *receive)
{
	_print_mutex.lock();
	printf("%s: %s\n", receive->_from_name.c_str(), receive->_msg.c_str());
	_print_mutex.unlock();
}

// 处理应答操作
void ChatClient::handle_response(Response *response)
{
	_print_mutex.lock();
	if (response->_request != "heartbeat") {
		printf("%s: %s\n", response->_request.c_str(), response->_result.c_str());
	}
	_print_mutex.unlock();
}

// 登录
bool ChatClient::to_login(const std::string& name, const std::string& pwd)
{
	std::string xml = ChatXml::build_login_xml(name, pwd);

	Packet *pack = _tp->encode_pack(xml.c_str(), xml.length());
	if (!pack) {
		return false;
	}

	bool ret = _conn->post_packet(pack);
	if (!ret) {
		printf("%s:%d\n", __FILE__, __LINE__);
		delete pack;
	}

	return true;
}

// 注销
bool ChatClient::to_logout(const std::string& name)
{
	std::string xml = ChatXml::build_logout_xml(name);

	Packet *pack = _tp->encode_pack(xml.c_str(), xml.length());
	if (!pack) {
		return false;
	}

	bool ret = _conn->post_packet(pack);
	if (!ret) {
		delete pack;
	}

	return true;
}

// 发消息
bool ChatClient::to_send_msg(const std::string& from_name, const std::list<std::string>& tousers)
{
	std::string xml = ChatXml::build_send_xml("Hi. How are you.", from_name, tousers);

	Packet *pack = _tp->encode_pack(xml.c_str(), xml.length());
	if (!pack) {
		return false;
	}

	bool ret = _conn->post_packet(pack);
	if (!ret) {
		delete pack;
	}

	return true;
}

// 群发消息
bool ChatClient::to_groupsend(const std::string& from_name)
{
	std::string xml = ChatXml::build_groupsend_xml("Hi. How are you.", from_name);

	Packet *pack = _tp->encode_pack(xml.c_str(), xml.length());
	if (!pack) {
		return false;
	}

	bool ret = _conn->post_packet(pack);
	if (!ret) {
		delete pack;
	}

	return true;
}

// 拆分字符串
std::list<std::string> ChatClient::split(const std::string& source, const std::string& spliter)
{
	std::list<std::string> target;

	int offset = 0;
	size_t pos = source.find(spliter, offset);
	while (pos != std::string::npos) {
		std::string child = source.substr(offset, pos - offset);
		target.push_back(child);
		offset = pos + spliter.length();

		pos = source.find(spliter, offset);
	}

	if ((size_t)offset != source.length()) {
		target.push_back(source.substr(offset));
	}

	return target;
}
// 线程处理函数
void ChatClient::run(TBThread *thread, void *arg)
{
	int count = 0;

	for (;;) {
		if (!_running) {
			break;
		}

		sleep(1);
		++count;

		if (count >= 10) {
			// 发送心跳
			std::string xml = ChatXml::build_heartbeat_xml();
			Packet *pack = _tp->encode_pack(xml.c_str(), xml.length());
			if (!pack) {
				continue;
			}
			if (!_conn->post_packet(pack)) {
				delete pack;
			}

			// 重新计数
			count = 0;
		}
	}
}

// 停止线程
void ChatClient::stop_heartbeat()
{
	_running = false;
	_heartbeat_thread.join();
}

} /* namespace triones */
