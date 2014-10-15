/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_PACK_H__
#define __TRIONES_PACK_H__

#include <string>
#include <stdint.h>
#include "../comm/mutex.h"
#include "../comm/databuffer.h"

namespace triones
{

class Packet: public triones::DataBuffer
{
public:
	Packet();
	virtual ~Packet();

	// 璇诲彇灏忎簬255瀛楃涓插舰鐨勬暟鎹�
	bool read_str(std::string& s);
	// 鍐欏叆灏忎簬255闀垮害鐨勬暟鎹�
	bool write_str(const std::string& s);
	// 璇诲瓧绗︿覆褰㈢殑鏁版嵁
	bool read_string(std::string& s);
	// 鍐欏叆瀛楃涓插舰鐨勬暟鎹�
	bool write_string(const std::string& s);
	// 璇诲彇鏃堕棿
	bool read_time(uint64_t& t);
	// 鍐欏叆鏃堕棿鐨勬暟鎹�
	bool write_time(uint64_t n);

public:
	// 鐢ㄤ簬灏嗚妭鐐逛覆鍏ュ埌閾捐〃
	Packet *_next;
	// 鏁版嵁绫诲瀷
	int _type;

	void *_ioc;
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

public:
	Mutex  *_mutex;
	Packet *_head; // 閾惧ご
	Packet *_tail; // 閾惧熬
	int     _size; // 鍏冪礌鏁伴噺
};

} // namespace triones

#endif // #ifndef __TRIONES_PACK_H__
