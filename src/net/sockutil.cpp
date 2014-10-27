/******************************************************
 *   FileName: sockutil.cpp
 *     Author: triones  2014-10-27
 *Description:
 *******************************************************/

#include "cnet.h"
#include "sockutil.h"

namespace triones
{

//将网络地址转换为一个64位的无符号整型，方便key值的的操作
uint64_t sockutil::sock_addr2id(struct sockaddr_in *sockaddr)
{
	seriaddr id;
	id.sockaddr.family =  sockaddr->sin_family;
	id.sockaddr.port = sockaddr->sin_port;
	id.sockaddr.host = sockaddr->sin_addr.s_addr;

	return id.sockid;
}

//将ID回转为网络地址类型
void sockutil::sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr)
{
	seriaddr addr;
	addr.sockid = sockid;
	sockaddr->sin_family = addr.sockaddr.family;
	sockaddr->sin_port = addr.sockaddr.port;
	sockaddr->sin_addr.s_addr = addr.sockaddr.host;

	return;
}

//将网络地址转为字符串
std::string sockutil::sock_addr2str(struct sockaddr_in *sockaddr)
{
    char dest[32];
    unsigned long ad = ntohl(sockaddr->sin_addr.s_addr);
	const char *type = ntohs(sockaddr->sin_family) == SOCK_STREAM ? "tcp" : "udp";
    sprintf(dest, "%s:%d.%d.%d.%d:%d", type,
            static_cast<int>((ad >> 24) & 255),
            static_cast<int>((ad >> 16) & 255),
            static_cast<int>((ad >> 8) & 255),
            static_cast<int>(ad & 255),
            ntohs(sockaddr->sin_port));
    return dest;
}

//将网络ID转为字符串 example:10692856960556924930 -> udp:192.168.100.148:4000
std::string sockutil::sock_id2str(uint64_t id)
{
	sockaddr_in sockaddr;
	sock_id2addr(id, &sockaddr);
	return sock_addr2str(&sockaddr);
}

} /* namespace triones */
