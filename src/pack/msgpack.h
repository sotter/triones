/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_MSGPACK_H__
#define __TRIONES_MSGPACK_H__

#include "../comm/ref.h"
#include "./pack.h"

namespace triones
{

#pragma pack(1)
const uint16_t MSG_VERSION = 0x0001;
class MsgHeader
{
public:
	MsgHeader()
	{
		_ver = MSG_VERSION;
		_type = 0;
		_seq = _len = 0;
	}
	~MsgHeader()
	{
	}

	bool unpack(Packet *pack)
	{
		if ((int) sizeof(MsgHeader) > pack->getDataLen())
		{
			return false;
		}

		if (!pack->readInt16(_ver)) return false;
		if (!pack->readInt16(_type)) return false;
		if (!pack->readInt32(_seq)) return false;
		if (!pack->readInt32(_len)) return false;

		if ((int) _len > pack->getDataLen())
		{
			return false;
		}

		return true;
	}

	void pack(Packet *pack)
	{
		pack->writeInt16(_ver);
		pack->writeInt16(_type);
		pack->writeInt32(_seq);
		pack->writeInt32(_len);
	}

public:
	uint16_t _ver;
	uint16_t _type;
	uint32_t _seq;
	uint32_t _len;
};

class MsgPacket: public Ref
{
public:
	MsgPacket()
	{
	}

	virtual ~MsgPacket()
	{
	}

	virtual void body(Packet *body) = 0;

	virtual bool unbody(Packet *pack) = 0;

public:

	bool unpack(Packet *pack)
	{
		if (!_header.unpack(pack))
		{
			return false;
		}

		return unbody(pack);
	}

	void pack(Packet *pack)
	{
		_header.pack(pack);
		body(pack);
		int len = pack->getDataLen() - (int) sizeof(MsgHeader);
		pack->fill_int32(len, 8);
	}

public:
	MsgHeader _header;
};

#pragma pack()

} // namespace triones

#endif // #ifndef __TRIONES_MSGPACK_H__
