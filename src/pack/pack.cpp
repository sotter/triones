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

// 璇诲彇灏忎簬255瀛楃涓插舰鐨勬暟鎹�
bool Packet::read_str(std::string& s)
{
	uint8_t n;

	n = readInt8();

//	if (!readInt8(n))
//		return false;

	s.resize(n);
	return readBytes((char*)s.data(), n);
}

// 鍐欏叆灏忎簬255闀垮害鐨勬暟鎹�
bool Packet::write_str(const std::string& s)
{
	uint8_t n = (uint8_t) s.size();
	writeInt8(n);

	if (n > 0)
	{
		writeBytes(s.data(), (int)n);
	}

	return true;
}

// 璇诲瓧绗︿覆褰㈢殑鏁版嵁
bool Packet::read_string(std::string& s)
{
	uint32_t len;

//	if (!readInt32(len))
//		return false;

	len = readInt32();

	// 娉ㄦ剰锛氫互涓嬩唬鐮佺洿鎺ユ搷浣滃熀绫荤殑搴曞眰鏁版嵁
	if (_pdata + len > _pfree)
	{
		return false;
	}

	s.append(_pdata, _pdata + len);
	_pdata += len;

	return true;
}

// 鍐欏叆瀛楃涓插舰鐨勬暟鎹�
bool Packet::write_string(const std::string &s)
{
	unsigned int n = s.length() ;
	writeInt32(n);

	if (n > 0)
	{
		writeBytes(s.c_str(), n);
	}

	return true;
}

// 璇诲彇鏃堕棿
bool Packet::read_time(uint64_t& t)
{
	t = readInt64();
	return true;
}

// 鍐欏叆鏃堕棿鐨勬暟鎹�
bool Packet::write_time(uint64_t n)
{
	writeInt64(n);
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
	{ // 鏄┖閾�
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
