/**
 * author: Triones
 * date  : 2014-09-03
 */
#include "../net/cnet.h"

#include "pack.h"

namespace triones
{

Packet::Packet(int type)
{
	_next = NULL;
	_type = type;
	_ioc = NULL;
	_raw_data = NULL;
}

Packet::~Packet()
{
	//packet销毁时减少对_ioc的引用计数
	if(_ioc != NULL)
	{
		_ioc->sub_ref();
		_ioc = NULL;
	}

	if (_raw_data) {
		delete _raw_data;
	}
}

bool Packet::read_str(std::string& s)
{
	uint8_t n;

	n = readInt8();

//	if (!readInt8(n))
//		return false;

	s.resize(n);
	return readBytes((char*) s.data(), n);
}

bool Packet::write_str(const std::string& s)
{
	uint8_t n = (uint8_t) s.size();
	writeInt8(n);

	if (n > 0)
	{
		writeBytes(s.data(), (int) n);
	}

	return true;
}

bool Packet::read_string(std::string& s)
{
	uint32_t len;

//	if (!readInt32(len))
//		return false;

	len = readInt32();

	if (_pdata + len > _pfree)
	{
		return false;
	}

	s.append(_pdata, _pdata + len);
	_pdata += len;

	return true;
}

bool Packet::write_string(const std::string &s)
{
	unsigned int n = s.length();
	writeInt32(n);

	if (n > 0)
	{
		writeBytes(s.c_str(), n);
	}

	return true;
}

bool Packet::read_time(uint64_t& t)
{
	t = readInt64();
	return true;
}

bool Packet::write_time(uint64_t n)
{
	writeInt64(n);
	return true;
}

void  Packet::set_type(int type)
{
	_type = type;
}

int  Packet::get_type()
{
	return _type;
}


void Packet::set_ioc(IOComponent *ioc)
{
	if(ioc == _ioc)
		return;

	//与原先的_ioc解绑
	if(_ioc != NULL)
	{
		_ioc->sub_ref();
		_ioc = NULL;
	}

	_ioc = ioc;
	if(_ioc != NULL)
	{
		_ioc->add_ref();
	}
}

IOComponent* Packet::get_ioc()
{
	return _ioc;
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
	if (NULL != _mutex)
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
	{
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
