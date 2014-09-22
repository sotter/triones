/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_PACK_H__
#define __TRIONES_PACK_H__

#include <string>
#include <stdint.h>
#include "../thread/mutex.h"
#include "../mem/databuffer.h"

namespace triones
{

class Packet: public DataBuffer
{
public:
	Packet();
	virtual ~Packet();

	// 读取小于255字符串形的数据
	bool read_str(std::string& s);
	// 写入小于255长度的数据
	bool write_str(const std::string& s);
	// 读字符串形的数据
	bool read_string(std::string& s);
	// 写入字符串形的数据
	bool write_string(const std::string& s);
	// 读取时间
	bool read_time(uint64_t& t);
	// 写入时间的数据
	bool write_time(uint64_t n);

public:
	// 用于将节点串入到链表
	Packet *_next;
	// 数据类型
	int _type;
};

class PacketQueue
{
public:
	PacketQueue();
	virtual ~PacketQueue();
	Packet* head();
	Packet* tail();
	Packet* pop();
	void    push(Packet *packet);
	void    clear();
	int     size();
	bool    empty();
	void    moveto(PacketQueue *destQueue);

protected:
	Mutex  *_mutex;
	Packet *_head; // 链头
	Packet *_tail; // 链尾
	int     _size; // 元素数量
};

} // namespace triones

#endif // #ifndef __TRIONES_PACK_H__
