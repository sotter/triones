/**
 * author: Triones
 * date  : 2014-09-03
 */
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include "tprotocol.h"

namespace triones
{
#define PACKET_MAX_SIZE    10240
#define TEXT_PACK_END      " \r\n"
#define TEXT_PACK_END_LEN  (3)
#define MIN_PACK_LEN       15
#define MAX_PACK_LEN       (4 * 1024)
#define COMM_MIN_PACK_LEN  (sizeof(MsgHeader))
#define COMM_MAX_PACK_LEN  (4 * 1024)

TransProtocolFac __trans_protocol;

TransProtocol::TransProtocol()
{
}

TransProtocol::~TransProtocol()
{
}

//data, len, 需要编码的数据，pack 编码后的结果。
bool TransProtocol::encode(const char *data, size_t len, PacketQueue *pack_queue)
{
	Packet *pack = encode_pack(data, len);
	if (pack != NULL)
	{
		pack_queue->push(pack);
		return true;
	}
	return false;
}

//socket 中读取到的数据，解析成的pakcet push到pack_queue中。
int TransProtocol::decode(const char *data, size_t len, PacketQueue *pack_queue)
{
	size_t decode_len;
	Packet *pack = NULL;
	size_t offset = 0;

	while ((pack = decode_pack(data + offset, len - offset, decode_len)) != NULL)
	{
		pack_queue->push(pack);
		offset += decode_len;
	}
	return offset;
}

Packet *TransProtocolText::encode_pack(const char *data, size_t len)
{
	if (NULL == data || len <= 0 || len > MAX_PACK_LEN)
	{
		return NULL;
	}

	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	pack->write_block(data, len);
	pack->write_block(TEXT_PACK_END, TEXT_PACK_END_LEN);

	return pack;
}

Packet *TransProtocolText::decode_pack(const char *data, size_t len, size_t &decode_len)
{
	decode_len = 0;
	if (NULL == data || len <= 0)
	{
		return NULL;
	}

	size_t offset = 0;				// 当前正在处理位置
	size_t start_pos = 0;			// 新包起始位置

	while (offset < len - 1)
	{
		if (data[offset] == '\r' && data[offset + 1] == '\n')
		{
			// 移动下次处理位置
			offset += 2;
			decode_len = offset;

			// 此次分包的长度
			int pack_len = offset - start_pos;

			// 跳过首部的"\r\n"，分包应该大于2个字节
			if (pack_len <= 2)
			{
				start_pos = offset;
				continue;
			}

			// 创建分包
			Packet *pack = new Packet;
			if (NULL == pack)
			{
				return NULL;
			}

			// 填写分包
			pack->write_block(data + start_pos, pack_len);

			return pack;
		}
		offset++;
	}

	return NULL;
}

//加密等放在一起了。把7d->7d 01, 7e->7e 02
Packet *TransProtocol808::encode_pack(const char *data, size_t len)
{
	// 因为下文用到了data[0]和data[len -1]，所以至少2个字节
	if (data == NULL || len <= 2 || len > MAX_PACK_LEN)
	{
		return NULL;
	}

	// 创建数据包
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	// 假设第0个字节为0x7e，不动它
	pack->write_int8(data[0]);

	// 从第1个字节开始处理
	for (size_t i = 1; i < len - 1; i++)
	{
		if (data[i] == 0x7e)
		{
			pack->write_int8(0x7d);
			pack->write_int8(0x02);
		}
		else if (data[i] == 0x7d)
		{
			pack->write_int8(0x7d);
			pack->write_int8(0x01);
		}
		else
		{
			pack->write_int8(data[i]);
		}
	}

	// 假设末尾字节为0x7e,不动它直接添加
	pack->write_int8(data[len - 1]);

	return pack;
}

Packet *TransProtocol808::decode808(const char *begin, const char *end)
{
	if (NULL == begin || NULL == end)
	{
		return NULL;
	}

	int len = end - begin;
	if (len < 0 || len > MAX_PACK_LEN)
	{
		return NULL;
	}

	// 创建分包
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	// 设置包头
	pack->write_int8(begin[0]);

	// 填充包内容
	unsigned char crc = 0;
	for (int i = 0; i < len - 1; i++)
	{
		if (begin[i] == 0x7d && begin[i + 1] == 0x02)
		{
			pack->write_int8(0x7e);
			crc ^= 0x7e;
			i++;
		}
		else if (begin[i] == 0x7d && begin[i + 1] == 0x01)
		{
			pack->write_int8(0x7d);
			crc ^= 0x7d;
			i++;
		}
		else
		{
			crc ^= begin[i];
			pack->write_int8(begin[i]);
		}
	}

	// 设置包尾
	pack->write_int8(begin[len - 1]);

	// 返回分包
	return pack;
}

//分包和解包放在一起了
Packet *TransProtocol808::decode_pack(const char *data, size_t len, size_t &decode_len)
{
	decode_len = 0;
	if (data == NULL || len <= 0)
	{
		return NULL;
	}

	size_t offset = 0;
	const char *begin = NULL;
	const char *end = NULL;
	const char *pbuf = data;

	//先分包，在验证包, 这样会遍历两遍, 如果在底层分包，上面在接解包，一样会遍历两遍。
	while (offset++ < len)
	{
		if (pbuf[offset] == 0x7e)
		{
			if (begin != NULL)
			{
				begin = pbuf + offset;
			}
			else
			{
				end = pbuf + offset;
				break;
			}
		}
	}

	//找到了包头，也找到了包尾
	if (begin != NULL && end != NULL)
	{
		decode_len = offset;
		return decode808(begin, end);
	}
	//找了包头，没有找到包尾
	else if (begin != NULL && end == NULL)
	{
		decode_len = begin - pbuf;
		return NULL;
	}
	//即没有找到包头，也没找到包尾, 认为是垃圾数据
	else
	{
		decode_len = offset;
		return NULL;
	}
}

static unsigned char CheckSum(unsigned char *data, size_t len)
{
	if (NULL == data || len <= 0)
	{
		return 0;
	}

	unsigned char sum = 0;
	for (size_t i = 0; i < len; i++)
	{
		sum ^= data[i];
	}

	return sum;
}

#pragma pack(1)
struct CTFOHEADER
{

#define CTFO_TAG            0x1adf5fed

	unsigned int ctfo_begin;
	unsigned int len;
	unsigned char checkbit;

	CTFOHEADER()
	:ctfo_begin(CTFO_TAG), len(0), checkbit(0)
	{
	}

	CTFOHEADER(unsigned int data_len)
	:ctfo_begin(CTFO_TAG)
	{
		len = htonl(data_len);
		checkbit = check_sum();
	}

	unsigned char check_sum()
	{
		return CheckSum((unsigned char*) &ctfo_begin, sizeof(ctfo_begin) + sizeof(len));
	}

	bool check_header()
	{
		return (ctfo_begin == CTFO_TAG) && (checkbit == check_sum());
	}
};
#pragma pack()

Packet *TransProtocolCtfo::encode_pack(const char *data, size_t len)
{
	if (data == NULL || len > PACKET_MAX_SIZE)
	{
		return NULL;
	}

	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	CTFOHEADER header(len);
	pack->write_block(&header, sizeof(CTFOHEADER));
	pack->write_block(data, len);

	return pack;
}

Packet *TransProtocolCtfo::decode_pack(const char *data, size_t len, size_t &decode_len)
{
	decode_len = 0;
	if (data == NULL || len < (int) sizeof(CTFOHEADER))
	{
		return NULL;
	}

	size_t offset = 0;
	CTFOHEADER *header = NULL;

	while (offset < len - sizeof(CTFOHEADER))
	{
		header = (CTFOHEADER*) (data + offset);
		if (header->check_header())
		{
			break;
		}
		else
		{
			offset++;
		}
	}

	//设置已经解析了的数据的位置
	decode_len = offset;

	//没有找到数据包头，直接返回
	if (offset == len - sizeof(CTFOHEADER))
	{
		return NULL;
	}

	header = (CTFOHEADER*) (data + offset);
	size_t data_len = ntohl(header->len);

	//一个包还没有完全解析完，直接返回
	if (len - decode_len < sizeof(CTFOHEADER) + data_len)
	{
		return NULL;
	}

	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}
	pack->write_block(data + offset, sizeof(CTFOHEADER) + data_len);

	decode_len += sizeof(CTFOHEADER) + data_len;
	return pack;
}

Packet *TransProtocolComm::encode_pack(const char *data, size_t len)
{
	if (NULL == data || len > COMM_MAX_PACK_LEN)
	{
		return NULL;
	}

	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	pack->write_block(data, len);
	return pack;
}

Packet *TransProtocolComm::decode_pack(const char *data, size_t len, size_t &decode_len)
{
	decode_len = 0;
	if (NULL == data || len < COMM_MIN_PACK_LEN)
	{
		return NULL;
	}

	size_t offset = 0;
	size_t start_pos = 0;
	size_t body_len = 0;
	unsigned short msg_ver = 0;			// 消息版本号

	while (offset < len - sizeof(MsgHeader))
	{
		MsgHeader *header = (MsgHeader*) (data + offset);
		msg_ver = ntohs(header->_ver);
		body_len = ntohl(header->_len);

		// 检测版本和长度合法性
		if (MSG_VERSION == msg_ver && body_len < COMM_MAX_PACK_LEN)
		{
			start_pos = offset;
			break;
		}

		offset++;
	}
	decode_len = offset;

	// 检测是否解析出包头
	if (offset >= len - sizeof(MsgHeader))
	{
		return NULL;
	}

	// 检测接收到的数据是否足够组成一个分包
	if (len - offset < sizeof(MsgHeader) + body_len)
	{
		return NULL;
	}

	// 创建数据包
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}
	pack->write_block(data + offset, sizeof(MsgHeader) + body_len);

	decode_len += sizeof(MsgHeader) + body_len;
	return pack;
}

TransProtocolFac::TransProtocolFac()
{
	//DEFAULT_TPROTOCOL, TEXT_TPROTOCOL, CTFO_TPROTOCOL,
	_tp[TPROTOCOL_TEXT] = new TransProtocolText;
	_tp[TPROTOCOL_808] = new TransProtocol808;
	_tp[TPROTOCOL_COMM] = new TransProtocolComm;
}

TransProtocolFac::~TransProtocolFac()
{
	for (int i = 0; i < TPROTOCOL_NUM; i++)
	{
		delete _tp[i];
	}
}

TransProtocol* TransProtocolFac::get(int type)
{
	return _tp[type];
}

} // namespace triones


