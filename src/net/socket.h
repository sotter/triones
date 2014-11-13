#ifndef TBNET_SOCKET_H_
#define TBNET_SOCKET_H_
#include <string>
#include "../comm/mutex.h"

namespace triones
{

class Socket
{

public:
	enum{
		TRIONES_SOCK_TCP, TRIONES_SOCK_UDP
	};

	Socket();

	virtual ~Socket();

	// 连接到_address上
	bool connect(const char *host, const unsigned short port, int type);

	// 向已经绑定_address， 提供给重连使用
	bool connect();

	// 建立ACCEPTOR套结字，对于UDP来说仅仅是绑定address，没有listen的过程
	bool listen(const char *host, const unsigned short port, int type);

	//设置本地地址
	bool set_conn_addess(const char *host, unsigned short port, int type);

	//由外部调用设置本地地址
	bool set_address(struct sockaddr_in &addr);

	//由外部调用设置对端地址
	bool set_peer_address(struct sockaddr_in &addr);

	//创建套接字
	bool socket_create(int type);

	//根据已经的外部条件构造socket,提供给TCPServer UDPServer派生出来的客户端使用
	bool setup(int fd, struct sockaddr_in *addr, struct sockaddr_in *peer_addr);

	// 关闭套接字
	void close();

	// 关闭读写
	void shutdown();

	// 返回文件句柄
	int get_fd();

	// 返回IOComponent
	IOComponent *get_ioc();

	// 设置IOComponent
	void set_ioc(IOComponent *ioc);

	// 写数据
	int write(const void *data, int len);

	// UDP数据发送
	int sendto(const void *data, int len, sockaddr_in &dest);

	// 读数据
	int read(void *data, int len);

	// UDP数据读取
	int recvfrom(void *data, int len, sockaddr_in &src);

	// 获取本地的address生成的ID
	uint64_t get_sockid();

	// 获取对端address生成的ID
	uint64_t get_peer_sockid();

	// 获取_address对应的字符串形式
	std::string get_addr();

	bool set_keep_alive(bool on);

	bool set_reuse_addr(bool on);

	bool set_solinger(bool on, int seconds);

	bool set_tcp_nodelay(bool nodelay);

	bool set_tcp_quick_ack(bool quick_ack);

	bool set_int_option(int option, int value);

	bool set_time_option(int option, int milliseconds);

	bool set_so_blocking(bool on);

	int get_soerror();

	// 得到最后错误
	static int get_last_error()
	{
		return errno;
	}

private:

	bool get_address(const char *host, unsigned short port,
			struct sockaddr_in &dest);

protected:
	// socket 套接字
	int _fd;

	// socket本端的地址
	struct sockaddr_in _address;

	// socket对端的地址，对于UDP来说这个对端地址是可变的
	struct sockaddr_in _peer_address;

	// 所属于的IOComponent
	IOComponent *_iocomponent;

	//　多实例用一个dnsMutex
	static triones::Mutex _dns_mutex;
};
}

#endif /*SOCKET_H_*/
