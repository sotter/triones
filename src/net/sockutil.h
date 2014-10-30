/******************************************************
 *   FileName: sockutil.h
 *     Author: triones  2014-10-27
 *Description:
 *******************************************************/

#ifndef SOCKUTIL_H_
#define SOCKUTIL_H_

#include <string>

namespace triones
{

class sockutil
{
private:
	struct triones_sockaddr
	{
		unsigned short family;
		unsigned short port;
		unsigned int host;
	};

	union seriaddr
	{
		uint64_t sockid;
		triones_sockaddr sockaddr;
	};

public:
	//将网络地址转换为一个64位的无符号整型的标识ID，方便key值的的操作,如果作为string性能很低
	static uint64_t sock_addr2id(struct sockaddr_in *sockaddr);

	//将ID回转为网络地址类型
	static void sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr);

	//将网络地址转为字符串
	static std::string sock_addr2str(struct sockaddr_in *sockaddr);

	//将网络ID转为字符串 example:10692856960556924930 -> udp:192.168.100.148:4000
	static std::string sock_id2str(uint64_t id);
};

} /* namespace triones */
#endif /* SOCKUTIL_H_ */
