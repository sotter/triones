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

class IOComponent;

class Packet: public triones::DataBuffer
{
public:

	Packet(int type = 0);

	virtual ~Packet();

	bool read_str(std::string& s);

	bool write_str(const std::string& s);

	bool read_string(std::string& s);

	bool write_string(const std::string& s);

	bool read_time(uint64_t& t);

	bool write_time(uint64_t n);

	void set_type(int type);

	int  get_type();

	void set_ioc(IOComponent *ioc);

	IOComponent* get_ioc();

public:
	Packet *_next;
	int _type;
	IOComponent *_ioc;
};

class PacketQueue
{
public:

	PacketQueue();

	virtual ~PacketQueue();

	Packet* head();

	Packet* tail();

	Packet* pop();

	void push(Packet *packet);

	void clear();

	int size();

	bool empty();

	void moveto(PacketQueue *destQueue);

public:

	Mutex *_mutex;
	Packet *_head;
	Packet *_tail;
	int _size;
};

} // namespace triones

#endif // #ifndef __TRIONES_PACK_H__
