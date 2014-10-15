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

//data, len, 闇�缂栫爜鐨勬暟鎹紝pack 缂栫爜鍚庣殑缁撴灉銆�
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

//socket 涓鍙栧埌鐨勬暟鎹紝瑙ｆ瀽鎴愮殑pakcet push鍒皃ack_queue涓�
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

	pack->writeBytes(data, len);
	pack->writeBytes(TEXT_PACK_END, TEXT_PACK_END_LEN);

	return pack;
}

Packet *TransProtocolText::decode_pack(const char *data, size_t len, size_t &decode_len)
{
	decode_len = 0;
	if (NULL == data || len <= 0)
	{
		return NULL;
	}

	size_t offset = 0;				// 褰撳墠姝ｅ湪澶勭悊浣嶇疆
	size_t start_pos = 0;			// 鏂板寘璧峰浣嶇疆

	while (offset < len - 1)
	{
		if (data[offset] == '\r' && data[offset + 1] == '\n')
		{
			// 绉诲姩涓嬫澶勭悊浣嶇疆
			offset += 2;
			decode_len = offset;

			// 姝ゆ鍒嗗寘鐨勯暱搴�
			int pack_len = offset - start_pos;

			// 璺宠繃棣栭儴鐨�\r\n"锛屽垎鍖呭簲璇ュぇ浜�涓瓧鑺�
			if (pack_len <= 2)
			{
				start_pos = offset;
				continue;
			}

			// 鍒涘缓鍒嗗寘
			Packet *pack = new Packet;
			if (NULL == pack)
			{
				return NULL;
			}

			// 濉啓鍒嗗寘
			pack->writeBytes(data + start_pos, pack_len);

			return pack;
		}
		offset++;
	}

	return NULL;
}

//鍔犲瘑绛夋斁鍦ㄤ竴璧蜂簡銆傛妸7d->7d 01, 7e->7e 02
Packet *TransProtocol808::encode_pack(const char *data, size_t len)
{
	// 鍥犱负涓嬫枃鐢ㄥ埌浜哾ata[0]鍜宒ata[len -1]锛屾墍浠ヨ嚦灏�涓瓧鑺�
	if (data == NULL || len <= 2 || len > MAX_PACK_LEN)
	{
		return NULL;
	}

	// 鍒涘缓鏁版嵁鍖�
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	// 鍋囪绗�涓瓧鑺備负0x7e锛屼笉鍔ㄥ畠
	pack->writeInt8(data[0]);

	// 浠庣1涓瓧鑺傚紑濮嬪鐞�
	for (size_t i = 1; i < len - 1; i++)
	{
		if (data[i] == 0x7e)
		{
			pack->writeInt8(0x7d);
			pack->writeInt8(0x02);
		}
		else if (data[i] == 0x7d)
		{
			pack->writeInt8(0x7d);
			pack->writeInt8(0x01);
		}
		else
		{
			pack->writeInt8(data[i]);
		}
	}

	// 鍋囪鏈熬瀛楄妭涓�x7e,涓嶅姩瀹冪洿鎺ユ坊鍔�
	pack->writeInt8(data[len - 1]);

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

	// 鍒涘缓鍒嗗寘
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}

	// 璁剧疆鍖呭ご
	pack->writeInt8(begin[0]);

	// 濉厖鍖呭唴瀹�
	unsigned char crc = 0;
	for (int i = 0; i < len - 1; i++)
	{
		if (begin[i] == 0x7d && begin[i + 1] == 0x02)
		{
			pack->writeInt8(0x7e);
			crc ^= 0x7e;
			i++;
		}
		else if (begin[i] == 0x7d && begin[i + 1] == 0x01)
		{
			pack->writeInt8(0x7d);
			crc ^= 0x7d;
			i++;
		}
		else
		{
			crc ^= begin[i];
			pack->writeInt8(begin[i]);
		}
	}

	// 璁剧疆鍖呭熬
	pack->writeInt8(begin[len - 1]);

	// 杩斿洖鍒嗗寘
	return pack;
}

//鍒嗗寘鍜岃В鍖呮斁鍦ㄤ竴璧蜂簡
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

	//鍏堝垎鍖咃紝鍦ㄩ獙璇佸寘, 杩欐牱浼氶亶鍘嗕袱閬� 濡傛灉鍦ㄥ簳灞傚垎鍖咃紝涓婇潰鍦ㄦ帴瑙ｅ寘锛屼竴鏍蜂細閬嶅巻涓ら亶銆�
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

	//鎵惧埌浜嗗寘澶达紝涔熸壘鍒颁簡鍖呭熬
	if (begin != NULL && end != NULL)
	{
		decode_len = offset;
		return decode808(begin, end);
	}
	//鎵句簡鍖呭ご锛屾病鏈夋壘鍒板寘灏�
	else if (begin != NULL && end == NULL)
	{
		decode_len = begin - pbuf;
		return NULL;
	}
	//鍗虫病鏈夋壘鍒板寘澶达紝涔熸病鎵惧埌鍖呭熬, 璁や负鏄瀮鍦炬暟鎹�
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
	pack->writeBytes(&header, sizeof(CTFOHEADER));
	pack->writeBytes(data, len);

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

	//璁剧疆宸茬粡瑙ｆ瀽浜嗙殑鏁版嵁鐨勪綅缃�
	decode_len = offset;

	//娌℃湁鎵惧埌鏁版嵁鍖呭ご锛岀洿鎺ヨ繑鍥�
	if (offset == len - sizeof(CTFOHEADER))
	{
		return NULL;
	}

	header = (CTFOHEADER*) (data + offset);
	size_t data_len = ntohl(header->len);

	//涓�釜鍖呰繕娌℃湁瀹屽叏瑙ｆ瀽瀹岋紝鐩存帴杩斿洖
	if (len - decode_len < sizeof(CTFOHEADER) + data_len)
	{
		return NULL;
	}

	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}
	pack->writeBytes(data + offset, sizeof(CTFOHEADER) + data_len);

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

	pack->writeBytes(data, len);
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
	unsigned short msg_ver = 0;			// 娑堟伅鐗堟湰鍙�

	while (offset < len - sizeof(MsgHeader))
	{
		MsgHeader *header = (MsgHeader*) (data + offset);
		msg_ver = ntohs(header->_ver);
		body_len = ntohl(header->_len);

		// 妫�祴鐗堟湰鍜岄暱搴﹀悎娉曟�
		if (MSG_VERSION == msg_ver && body_len < COMM_MAX_PACK_LEN)
		{
			start_pos = offset;
			break;
		}

		offset++;
	}
	decode_len = offset;

	// 妫�祴鏄惁瑙ｆ瀽鍑哄寘澶�
	if (offset >= len - sizeof(MsgHeader))
	{
		return NULL;
	}

	// 妫�祴鎺ユ敹鍒扮殑鏁版嵁鏄惁瓒冲缁勬垚涓�釜鍒嗗寘
	if (len - offset < sizeof(MsgHeader) + body_len)
	{
		return NULL;
	}

	// 鍒涘缓鏁版嵁鍖�
	Packet *pack = new Packet();
	if (NULL == pack)
	{
		return NULL;
	}
	pack->writeBytes(data + offset, sizeof(MsgHeader) + body_len);

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


