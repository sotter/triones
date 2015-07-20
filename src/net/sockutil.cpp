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
//第一个字节由is_server组成，client和server之间值肯定是不相等
//第二个字节表示sin_family， 在TCP/IP协议中family大小没有超过127
//第三个字节由sin_port组成
//第四个字节由sin_addr组成
//uint64_t sockutil::sock_addr2id(struct sockaddr_in *sockaddr, bool is_server)
//{
//	uint64_t sockid = is_server ? 1 : 0;
//	sockid <<= 8;
//	sockid |= sockaddr->sin_family;
//	sockid <<= 8;
//	sockid |= ntohs(sockaddr->sin_port);
//	sockid <<= 32;
//	sockid |= sockaddr->sin_addr.s_addr;
//
//	return sockid;
//}

uint64_t sockutil::sock_addr2id(struct sockaddr_in *sockaddr, bool is_tcp, bool is_server)
{
	uint64_t sockid = 0;

	sockid <<= 1;
	sockid |= (is_tcp ? 1 : 0);

	sockid <<= 1;
	sockid |= (is_server ? 1: 0);

	sockid <<= 16;
	sockid |= sockaddr->sin_port;

	sockid <<= 32;
	sockid |= sockaddr->sin_addr.s_addr;

	return sockid;
}

//将ID回转为网络地址类型
//void sockutil::sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr)
//{
//	sockaddr->sin_family = (sockid >> 48) && 0xff;
//	sockaddr->sin_port = htons(sockid >> 32);
//	sockaddr->sin_addr.s_addr = sockid;
//
//	return;
//}

void sockutil::sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr)
{
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = (sockid >> 32) & 0X0000FFFF;			// 取高32位的低16位
	sockaddr->sin_addr.s_addr = sockid & 0X00000000FFFFFFFF;	// 取低32位

	return;
}

//将网络地址转为字符串
std::string sockutil::sock_addr2str(struct sockaddr_in *sockaddr)
{
	char dest[32];
	unsigned long ad = ntohl(sockaddr->sin_addr.s_addr);
//	const char *type = (sockaddr->sin_family) == SOCK_STREAM ? "tcp" : "udp";
	sprintf(dest, "%d.%d.%d.%d:%d", static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(sockaddr->sin_port));
	return dest;
}

//将网络ID转为字符串 example:10692856960556924930 -> udp:192.168.100.148:4000
std::string sockutil::sock_id2str(uint64_t id)
{
	sockaddr_in sockaddr;
	sock_id2addr(id, &sockaddr);
	return sock_addr2str(&sockaddr);
}


//static uint8_t sockutil::get_seq()
//{
//	static uint8_t seq = 0;
//	return ++seq;
//}

} /* namespace triones */
