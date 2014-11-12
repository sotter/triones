
#ifndef TBNET_SOCKET_H_
#define TBNET_SOCKET_H_
#include <string>
#include "../comm/mutex.h"

namespace triones
{

class Socket
{

public:
	Socket();

	virtual ~Socket();

	//检查socket fd, 如果没有建立
	bool check_fd();

	//设置地址，对端的连接地址
	bool set_address(const char *address, const int port);

	//绑定udp地址
	bool udp_bind();

	//连接到_address上
	bool connect();

	//关闭套接字
	void close();

	// 关闭读写
	void shutdown();

	// 使用UDP的socket
	bool udp_create();

	// 把socketHandle,及ipaddress设置到此socket中
	void setup(int socketHandle, struct sockaddr *hostAddress);

	//返回文件句柄
	int get_fd();

	// 返回IOComponent
	IOComponent *get_ioc();

	// 设置IOComponent
	void set_ioc(IOComponent *ioc);

	//写数据
	int write(const void *data, int len);

	//UDP数据发送
	int sendto(const void *data, int len, sockaddr_in &dest);

	//读数据
	int read(void *data, int len);

	//UDP数据读取
	int recvfrom(void *data, int len, sockaddr_in &src);

	//获取本地的address生成的ID
	uint64_t get_sockid();

	//获取对端address生成的ID
	uint64_t get_peer_sockid();

	//获取_address对应的字符串形式
	std::string get_addr();

	bool set_keep_alive(bool on);

	bool set_reuse_addr(bool on);

	bool set_solinger(bool doLinger, int seconds);

	bool set_tcp_nodelay(bool noDelay);

	bool set_tcp_quick_ack(bool quickAck);

	bool set_int_option(int option, int value);

	bool set_time_option(int option, int milliseconds);

	bool set_so_blocking(bool on);

	int get_soerror();

	//得到最后错误
	static int get_last_error()
	{
		return errno;
	}

protected:
	struct sockaddr_in _address;

	int _fd;

	IOComponent *_iocomponent;

	//　多实例用一个dnsMutex
	static triones::Mutex _dnsMutex;
};
}

#endif /*SOCKET_H_*/
