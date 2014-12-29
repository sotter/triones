#ifndef TBNET_SOCKET_H_
#define TBNET_SOCKET_H_
#include <string>
#include "../comm/mutex.h"

namespace triones
{

class Socket
{

public:
	enum{ TRIONES_SOCK_TCP, TRIONES_SOCK_UDP };

	Socket();

	virtual ~Socket();

	//创建套接字
	bool socket_create(int type = TRIONES_SOCK_TCP);

	// 连接到_address上, 同步连接接口使用
	bool connect(const char *host, const unsigned short port, int type);

	// 向已经绑定_address， 提供给重连使用
	bool connect();

	// 建立ACCEPTOR套结字，对于UDP来说仅仅是绑定address，没有listen的过程
	bool listen(const char *host, const unsigned short port, int type);

	bool set_address(const char *host, unsigned short port, bool peer = true);

	//优先级：peer 表示是设置本端地址还是对端地址
	//(1) 外部传入的addr不为空，已外部传入的addr准
	//(2) 外部传入为空时，已通过系统调用获取的addr为准
	//(3) 在无法获取_fd的sockname时，已原先的为标准
	bool set_address(struct sockaddr_in *addr, bool peer = true);

	//根据已经的外部条件构造socket,提供给TCPServer UDPServer派生出来的client
	bool setup(int fd, struct sockaddr_in *addr = NULL, struct sockaddr_in *peer_addr = NULL, bool tcp = true);

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

	// 非阻塞读数据
	int nonblock_read(void *data, int len);

	// UDP数据读取
	int recvfrom(void *data, int len, sockaddr_in &src);

	// 非阻塞UDP数据读取
	int nonblock_recvfrom(void *data, int len, sockaddr_in &src);

	// 获取本地的address生成的ID
	uint64_t get_sockid(bool is_tcp);

	// 获取对端address生成的ID
	uint64_t get_peer_sockid(bool is_tcp);

	// 获取_address对应的字符串形式
	std::string get_addr();

	// 获取_peer_address对应的字符串形式
	std::string get_peer_addr();

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

	void show_addr();

protected:

	// socket 套接字
	int _fd;

#define ADDRINIT_FLAG (0x01)
#define PEERINIT_FLAG (0x02)
#define TCP_FLAG      (0x04)

	//标识_address, _peer_address是否已经出师化过
	uint8_t           _setup;

	// socket本端的地址
	struct sockaddr_in _address;

	// socket对端的地址，对于UDP来说这个对端地址是可变的
	struct sockaddr_in _peer_address;

	// 所属于的IOComponent
	IOComponent *_iocomponent;

	//　多实例用一个dnsMutex
	static triones::Mutex _dns_mutex;

	// 套接字类型：TRIONES_SOCK_TCP还是TRIONES_SOCK_UDP
	int _type;
};
}

#endif /*SOCKET_H_*/
