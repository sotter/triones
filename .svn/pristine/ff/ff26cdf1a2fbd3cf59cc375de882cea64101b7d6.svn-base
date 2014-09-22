/******************************************************
 *   FileName: fileclient.h
 *     Author: Triones  2012-9-17 
 *Description: 轻量级客户端，不要依赖额外的库。
 (1) 网络传输：
 数据发送和读取采用尝试发送，如果发送失败， socket缓冲区满时在启用select/epoll的 IO复用检测。
 同步操作接口，数据会话通过超时时间来确定，防止用户线程阻塞。
 （2）发送阻塞的条件：
 网络连接正常，网络没有连接的情况下并不会发生阻塞。 发生阻塞的可能性很小。
 由于服务端接收数据缓慢，导致的写缓冲满。实际上在应用层，每传输一条指令，都需要得到一个指令回复，才能进行下一步的操作，有流量控制的作用。
 服务端接收到数据后，没有给与响应。
 （3）用户接口：
 未暴露网络连接接口，数据操作时，如果发现网络未连接，自动建立连接。用户只需保存文件，和获取文件内容即可。
 暴露给用户的错误含义：
 网路错误、参数错误、文件操作错误。
 *******************************************************/

#ifndef FILECLIENT_H_
#define FILECLIENT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <stdlib.h>
#include "netop.h"
#include "eventpool.h"
#include "socket.h"

using namespace std;

#define FILE_SUCCESS             ( 0)
#define FILE_FAIL                (-1)

#define FILE_PARAM_ERROR         (-2)

#define SOCK_REMOTE_CLOSE        (-6)
#define SOCK_ERROR               (-7)
#define SOCK_TIMEOUT_ERROR       (-8)
#define SOCK_CONNECT_ERROR       (-9)

struct FileData
{
	char *data;
	int len;

	FileData()
	{
		data = NULL;
		len = 0;
	}

	~FileData()
	{
		if (data != NULL)
		{
			free(data);
			data = NULL;
		}
		len = 0;
	}
};

class FileClient;

enum USER_STATAE{OFF_LINE, ON_LINE};

class User
{
public:
	User(): _socket(NULL), _state(OFF_LINE){}

	Socket *_socket;
    int _state;
};

class FileClient : public NetService
{
public:

	virtual int on_connect(Socket *sock_event);

	virtual int on_close(Socket *sock_event);

	virtual int on_read(Socket *sock_event);

    FileClient(eventpool &pool) : _event_pool(pool){}

    bool start(const char *ip, unsigned short port);

    int put_file(const char *local_file, const char *remote_file);

	//将数据写入到远程文件
	int writefile(const char *data, int data_len, const char *remote_file);

private:
    User _user;
	eventpool &_event_pool;
};


#endif /* FILECLIENT_H_ */
