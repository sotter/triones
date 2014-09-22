/**
 * author: Triones
 * date  : 2014-09-03
 */
#include "pack.h"

namespace triones
{

Packet::Packet()
{
	_next = NULL;
	_type = 0;
}

Packet::~Packet()
{
}

// 读取小于255字符串形的数据
bool Packet::read_str(std::string& s)
{
	uint8_t n;

	if (!read_int8(n))
		return false;

	s.resize(n);
	return read_block((char*)s.data(), n);
}

// 写入小于255长度的数据
bool Packet::write_str(const std::string& s)
{
	uint8_t n = (uint8_t) s.size();
	write_int8(n);

	if (n > 0)
	{
		write_block(s.data(), (int)n);
	}

	return true;
}

// 读字符串形的数据
bool Packet::read_string(std::string& s)
{
	uint32_t len;

	if (!read_int32(len))
		return false;

	// 注意：以下代码直接操作基类的底层数据
	if (_pdata + len > _pfree)
	{
		return false;
	}

	s.append(_pdata, _pdata + len);
	_pdata += len;

	return true;
}

// 写入字符串形的数据
bool Packet::write_string(const std::string &s)
{
	unsigned int n = s.length() ;
	write_int32(n);

	if (n > 0)
	{
		write_block(s.c_str(), n);
	}

	return true;
}

// 读取时间
bool Packet::read_time(uint64_t& t)
{
	return read_int64(t);
}

// 写入时间的数据
bool Packet::write_time(uint64_t n)
{
	write_int64(n);
	return true;
}



//////////////////////////////////////////////////////////////////////
PacketQueue::PacketQueue()
{
	_head = NULL;
	_tail = NULL;
	_size = 0;
	_mutex = new Mutex;	
}

PacketQueue::~PacketQueue()
{
	clear();
	if(NULL != _mutex)
	{
		delete _mutex;
		_mutex = NULL;
	}
}

Packet *PacketQueue::head()
{
	return _head;
}

Packet* PacketQueue::tail()
{
	return _tail;
}

Packet *PacketQueue::pop()
{
	Guard g(*_mutex);

	if (_head == NULL)
	{
		return NULL;
	}
	Packet *packet = _head;
	_head = _head->_next;
	if (_head == NULL)
	{
		_tail = NULL;
	}
	_size--;
	return packet;
}

void PacketQueue::push(Packet *packet)
{
	Guard g(*_mutex);

	if (packet == NULL)
	{
		return;
	}
	packet->_next = NULL;
	if (_tail == NULL)
	{
		_head = packet;
	}
	else
	{
		_tail->_next = packet;
	}
	_tail = packet;
	_size++;
}

void PacketQueue::clear()
{
	Guard g(*_mutex);

	if (_head == NULL)
	{
		return;
	}

	while (_head != NULL)
	{
		Packet *packet = _head;
		_head = packet->_next;
		delete packet;
	}
	_head = _tail = NULL;
	_size = 0;
}

int PacketQueue::size()
{
	return _size;
}

bool PacketQueue::empty()
{
	return (_size == 0);
}

void PacketQueue::moveto(PacketQueue *destQueue)
{
	Guard g(*_mutex);

	if (_head == NULL)
	{ // 是空链
		return;
	}
	if (destQueue->_tail == NULL)
	{
		destQueue->_head = _head;
	}
	else
	{
		destQueue->_tail->_next = _head;
	}
	destQueue->_tail = _tail;
	destQueue->_size += _size;
	_head = _tail = NULL;
	_size = 0;
}

} // namespace triones
