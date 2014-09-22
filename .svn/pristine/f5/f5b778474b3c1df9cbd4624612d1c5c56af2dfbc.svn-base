/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_MSGPACK_H__
#define __TRIONES_MSGPACK_H__

#include "../thread/ref.h"
#include "pack.h"

namespace triones
{

#pragma pack(1)
const uint16_t MSG_VERSION = 0x0001; // 消息版本号

// 消息数据头
class MsgHeader
{
public:
	MsgHeader()
	{
		_ver  = MSG_VERSION;
		_type = 0;
		_seq  = _len = 0;
	}
	~MsgHeader()
	{
	}

	// 解包数据
	bool unpack(Packet *pack)
	{
		if ((int) sizeof(MsgHeader) > pack->size())
		{
			return false;
		}

		if (!pack->read_int16(_ver))  return false;
		if (!pack->read_int16(_type)) return false;
		if (!pack->read_int32(_seq))  return false;
		if (!pack->read_int32(_len))  return false;

		if ((int)_len > pack->size())
		{
			return false;
		}

		return true;
	}

	// 打包数据
	void pack(Packet *pack)
	{
		pack->write_int16(_ver);
		pack->write_int16(_type);
		pack->write_int32(_seq);
		pack->write_int32(_len);
	}

public:
	// 协议的版本号
	uint16_t _ver;
	// 协议的类型
	uint16_t _type;
	//　协议的序号
	uint32_t _seq;
	// 数据长度
	uint32_t _len;
};

// 数据解包对象接口
class MsgPacket: public Ref
{
public:
	MsgPacket()
	{
	}
	// 虚析构函数
	virtual ~MsgPacket()
	{
	}
	// 打包消息体
	virtual void body(Packet *body) = 0;
	// 解包数据体
	virtual bool unbody(Packet *pack) = 0;

public:
	// 解包数据
	bool unpack(Packet *pack)
	{
		if (!_header.unpack(pack))
		{
			return false;
		}

		return unbody(pack);
	}

	// 打包数据
	void pack(Packet *pack)
	{
		_header.pack(pack);
		body(pack);
		int len = pack->size() - (int) sizeof(MsgHeader);
		pack->fill_int32(len, 8);
	}

public:
	// 基本的数据头
	MsgHeader _header;
};

#pragma pack()

} // namespace triones

#endif // #ifndef __TRIONES_MSGPACK_H__
