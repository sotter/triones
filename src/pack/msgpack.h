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
const uint16_t MSG_VERSION = 0x0001; // 娑堟伅鐗堟湰鍙�

// 娑堟伅鏁版嵁澶�
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

	// 瑙ｅ寘鏁版嵁
	bool unpack(Packet *pack)
	{
		if ((int) sizeof(MsgHeader) > pack->getDataLen())
		{
			return false;
		}

		if (!pack->readInt16(_ver))  return false;
		if (!pack->readInt16(_type)) return false;
		if (!pack->readInt32(_seq))  return false;
		if (!pack->readInt32(_len))  return false;

		if ((int)_len > pack->getDataLen())
		{
			return false;
		}

		return true;
	}

	// 鎵撳寘鏁版嵁
	void pack(Packet *pack)
	{
		pack->writeInt16(_ver);
		pack->writeInt16(_type);
		pack->writeInt32(_seq);
		pack->writeInt32(_len);
	}

public:
	// 鍗忚鐨勭増鏈彿
	uint16_t _ver;
	// 鍗忚鐨勭被鍨�
	uint16_t _type;
	//銆�崗璁殑搴忓彿
	uint32_t _seq;
	// 鏁版嵁闀垮害
	uint32_t _len;
};

// 鏁版嵁瑙ｅ寘瀵硅薄鎺ュ彛
class MsgPacket: public Ref
{
public:
	MsgPacket()
	{
	}
	// 铏氭瀽鏋勫嚱鏁�
	virtual ~MsgPacket()
	{
	}
	// 鎵撳寘娑堟伅浣�
	virtual void body(Packet *body) = 0;
	// 瑙ｅ寘鏁版嵁浣�
	virtual bool unbody(Packet *pack) = 0;

public:
	// 瑙ｅ寘鏁版嵁
	bool unpack(Packet *pack)
	{
		if (!_header.unpack(pack))
		{
			return false;
		}

		return unbody(pack);
	}

	// 鎵撳寘鏁版嵁
	void pack(Packet *pack)
	{
		_header.pack(pack);
		body(pack);
		int len = pack->getDataLen() - (int) sizeof(MsgHeader);
		pack->fill_int32(len, 8);
	}

public:
	// 鍩烘湰鐨勬暟鎹ご
	MsgHeader _header;
};

#pragma pack()

} // namespace triones

#endif // #ifndef __TRIONES_MSGPACK_H__
