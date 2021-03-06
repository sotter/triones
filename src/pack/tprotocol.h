/**
 * author: Triones
 * date  : 2014-09-03
 */
#ifndef __TRIONES_TPROTOCOL_H__
#define __TRIONES_TPROTOCOL_H__

#include "msgpack.h"

namespace triones
{
/***********************************
 * client 使用流程：
 * （1）设置Transport protocol.
 * (2)send data, TransPortprotocol 编码放到send_packqueue 中，由epoll的write驱动发送。
 * (3)recv data, TransPortPortocol 解码放到read_packqueue 中，
 *
 * 采用这种方式，总体上将数据多了一次拷贝的过程。read 出来后，由split放到
 *
 * 内存拷贝的速率测试。
 ***********************************/

class TransProtocol;

enum TRANS_PROTOCOL_TYPE
{
	TPROTOCOL_TEXT, TPROTOCOL_808, TPROTOCOL_COMM, TPROTOCOL_NUM
};

class TransProtocol
{
public:
	TransProtocol();
	virtual ~TransProtocol();
	//encode出一个包来
	virtual Packet *encode_pack(const char *data, size_t len)= 0;
	//解析出一个包来，其中返回中也包含未解析出来的数据
	virtual Packet *decode_pack(const char *data, size_t len, size_t &decode_len) = 0;
	//data, len, 需要编码的数据，pack 编码后的结果。
	bool encode(const char *data, size_t len, PacketQueue *pack_queue);
	//socket 中读取到的数据，解析成的pakcet push到pack_queue中。
	int decode(const char *data, size_t len, PacketQueue *pack_queue);
};

extern TransProtocol * tprotocol_new(int TYPE);

class TransProtocolText: public TransProtocol
{
public:
	//encode出一个包来
	virtual Packet *encode_pack(const char *data, size_t len);
	//解析出一个包来
	virtual Packet *decode_pack(const char *data, size_t len, size_t &decode_len);
};

class TransProtocol808: public TransProtocol
{
public:
	virtual Packet *encode_pack(const char *data, size_t len);
	//解析出一个包来
	virtual Packet *decode_pack(const char *data, size_t len, size_t &decode_len);
private:

	Packet *decode808(const char *begin, const char *end);
};

class TransProtocolComm: public TransProtocol
{
public:
	virtual Packet *encode_pack(const char *data, size_t len);
	//解析出一个包来
	virtual Packet *decode_pack(const char *data, size_t len, size_t &decode_len);
};

class TransProtocolCtfo: public TransProtocol
{
public:
	virtual Packet *encode_pack(const char *data, size_t len);
	//解析出一个包来
	virtual Packet *decode_pack(const char *data, size_t len, size_t &decode_len);
};

//相当于静态存储
class TransProtocolFac
{
public:
	TransProtocolFac();
	~TransProtocolFac();
	TransProtocol *get(int type);

private:
	TransProtocol* _tp[TPROTOCOL_NUM];
};

extern TransProtocolFac __trans_protocol;
} // namespace triones

#endif // #ifndef __TRIONES_TPROTOCOL_H__
