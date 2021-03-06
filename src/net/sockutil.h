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

public:
	//将网络地址转换为一个64位的无符号整型的标识ID，方便key值的的操作,如果作为string性能很低
	//static uint64_t sock_addr2id(struct sockaddr_in *sockaddr, bool is_server = false);
	static uint64_t sock_addr2id(struct sockaddr_in *sockaddr, bool is_tcp, bool is_server);

	//将ID回转为网络地址类型
	static void sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr);

	//将网络地址转为字符串
	static std::string sock_addr2str(struct sockaddr_in *sockaddr);

	//将网络ID转为字符串 example:10692856960556924930 -> udp:192.168.100.148:4000
	static std::string sock_id2str(uint64_t id);


private:
//	//为了最大程度的避免sockid产生重复，将sockid的高16置为一个序列值。
//	static uint8_t get_seq();
};

} /* namespace triones */
#endif /* SOCKUTIL_H_ */
