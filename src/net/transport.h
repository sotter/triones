
#ifndef TBNET_TRANSPORT_H_
#define TBNET_TRANSPORT_H_

#include <sys/ptrace.h>
#include "../comm/thread.h"

namespace triones
{

class Transport: public triones::TBRunnable
{

public:

	friend class UDPAcceptor;

	Transport();

	~Transport();

	//起动运输层，创建两个线程，一个用于读，一个用写。
	bool start();

	//停止，停掉读写线程，及销毁。
	bool stop();

	//等待线程完全退出
	bool wait();

	//线程的运行函数，实现Runnable接口中的函数
	void run(triones::TBThread *thread, void *arg);

	/*
	 * 起一个监听端口。
	 * @param spec: 格式 [upd|tcp]:ip:port
	 * @param streamer: 数据包的双向流，用packet创建，解包，组包。
	 * @param serverAdapter: 用在服务器端，当Connection初始化及Channel创建时回调时用
	 * @return IO组件一个对象的指针
	 */
	IOComponent *listen(const char *spec, triones::TransProtocol *streamer,
	        IServerAdapter *serverAdapter);

	/*
	 * 创建一个Connection，连接到指定的地址，并加入到Socket的监听事件中。
	 *
	 * @param spec: 格式 [upd|tcp]:ip:port
	 * @param streamer: 数据包的双向流，用packet创建，解包，组包。
	 * @param autoReconn: 是否重连
	 */
	IOComponent *connect(const char *spec, triones::TransProtocol *streamer,
	        bool autoReconn = false);

	void add_component(IOComponent *ioc, bool readOn, bool writeOn);

	// 从iocomponents中删除掉
	void remove_component(IOComponent *ioc);

	IOComponent *get_component(uint64_t id)
	{
		return _hash_socks->get(id);
	}

	// 是否为stop
	bool* getStop();

private:

	//把[upd|tcp]:ip:port分开放在args中,cnt:数组中最大个数, return返回的数组中个数
	int parse_addr(char *src, char **args, int cnt);

	// socket event 的检测
	void event_loop(SocketEvent *socketEvent);

	//超时检查
	void timeout_loop();

	//释放变量
	void destroy();

private:
	// 是否被停止
	bool _stop;
	// 读写socket事件
	SocketEvent _sock_event;
	// 读写处理线程
	triones::TBThread _io_thread;
	// 超时检查线程
	triones::TBThread _timeout_thread;
	// transport采用hash列表
	HashSock    *_hash_socks;

	triones::Mutex _iocs_mutex;
};

}

#endif /*TRANSPORT_H_*/
