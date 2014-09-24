/**
 * author: Triones
 * date  : 2014-08-12
 * desc  : 数据缓存对象，主要处理数据内存自动开辟和数字形数据大端处理
 */

#ifndef __TRIONES_DATABUFFER_H__
#define __TRIONES_DATABUFFER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "netorder.h"

namespace triones
{

class DataBuffer
{
public:
	DataBuffer()
	:_ronly(0)
	{
		_pbuf = _pdata = _pfree = _pend = NULL;
	}

	virtual ~DataBuffer()
	{
		if (_pbuf && _ronly == 0)
		{
			free(_pbuf);
		}
		_pbuf = _pdata = _pfree = _pend = NULL;
	}

	// 取得数据
	char * data()
	{
		return (char*) _pbuf;
	}

	// 取得实际数据长度
	int size()
	{
		return (int)(_pfree - _pdata);
	}

	/*
	 * 写函数
	 */
	void write_int8(uint8_t n)
	{
		expand(1);
		*(_pfree++) = n;
	}

	/**
	 * 写短整形数据
	 */
	void write_int16(uint16_t n)
	{
		expand(2);
		n = htons(n);
		memcpy(_pfree, &n, 2);
		_pfree += 2;
	}

	/*
	 * 写出整型
	 */
	void write_int32(uint32_t n)
	{
		expand(4);
		n = htonl(n);
		memcpy(_pfree, &n, 4);
		_pfree += 4;
	}

	/**
	 * 写64的数据
	 */
	void write_int64(uint64_t n)
	{
		expand(8);
		n = htonll(n);
		memcpy(_pfree, &n, 8);
		_pfree += 8;
	}

	/**
	 * 写数据块
	 */
	void write_block(const void *src, int len)
	{
		expand(len);
		memcpy(_pfree, src, len);
		_pfree += len;
	}

	// 处理固定长度的数据
	void write_bytes(const void *src, int len, int max)
	{
		expand(max);
		memset(_pfree, 0, max);
		memcpy(_pfree, src, (len > max) ? max : len);
		_pfree += max;
	}

	// 填充指定的字符数据
	void write_fill(int c, int len)
	{
		expand(len);
		memset(_pfree, c, len);
		_pfree += len;
	}

	// 写指定的位置数据
	void fill_int8(uint8_t n, int offset)
	{
		*(_pbuf + offset) = n;
	}

	// 写指定位置的短整数据
	void fill_int16(uint16_t n, int offset)
	{
		n = htons(n);
		memcpy(_pbuf + offset, &n, 2);
	}

	void fill_int32(uint32_t n, int offset)
	{
		n = htonl(n);
		memcpy(_pbuf + offset, &n, 4);
	}

	void fill_int64(uint64_t n, int offset)
	{
		n = htonll(n);
		memcpy(_pbuf + offset, &n, 8);
	}

	// 写指定位置的内存块
	void fill_block(const void *src, int len, int offset)
	{
		memcpy(_pbuf + offset, src, len);
	}

	/*
	 * 读函数
	 */
	bool read_int8(uint8_t& i)
	{
		if (_pdata + 1 > _pfree)
			return false;

		i = *_pdata;
		_pdata += 1;

		return true;
	}

	bool read_int16(uint16_t& i)
	{
		uint16_t n = 0;
		if (_pdata + 2 > _pfree)
			return false;

		memcpy(&n, _pdata, 2);
		_pdata += 2;
		i = ntohs(n);

		return true;
	}

	bool read_int32(uint32_t& i)
	{
		uint32_t n = 0;
		if (_pdata + 4 > _pfree)
			return false;

		memcpy(&n, _pdata, 4);
		_pdata += 4;
		i = ntohl(n);

		return true;
	}

	bool read_int64(uint64_t& i)
	{
		uint64_t n = 0;
		if (_pdata + 8 > _pfree)
			return n;

		memcpy(&n, _pdata, 8);
		_pdata += 8;
		i = ntohll(n);

		return true;
	}

	// 读取数据块
	bool read_block(void *dst, int len)
	{
		if (_pdata + len > _pfree)
		{
			return false;
		}

		memcpy(dst, _pdata, len);
		_pdata += len;

		return true;
	}

	// 从指定位置数据
	uint8_t fetch_int8(int offset)
	{
		return *(_pbuf + offset);
	}

	uint16_t fetch_int16(int offset)
	{
		uint16_t n = 0;
		memcpy(&n, _pbuf + offset, 2);
		return ntohs(n);
	}

	uint32_t fetch_int32(int offset)
	{
		uint32_t n = 0;
		memcpy(&n, _pbuf + offset, 4);
		return ntohl(n);
	}

	uint64_t fetch_int64(int offset)
	{
		uint64_t n = 0;
		memcpy(&n, _pbuf + offset, 8);
		return ntohll(n);
	}

	// 读取指定长度数据块
	bool fetch_block(int offset, void *dst, int len)
	{
		// 判断错误处理
		if (offset < 0 || len < 0 || (_pfree - _pbuf) < len)
		{
			return false;
		}

		memcpy(dst, _pbuf + offset, len);
		return true;
	}

	// 重新定位数据处理
	void seek_pos(int offset)
	{
		_pdata = _pbuf + offset;
	}

	// 回退一段内存块
	void free_pos(int offset)
	{
		if (_pfree == NULL)
			return;
		_pfree = _pfree - offset;
		if (_pfree < _pbuf)
		{
			_pfree = _pbuf;
		}
	}

	// 重置数据空间
	void reset_buf(void)
	{
		// 将所有数据归零
		memset(_pbuf, 0, (_pfree - _pbuf));
		_pfree = _pdata = _pbuf;
	}

	// 写入一个BUF
	void write_buffer(DataBuffer &buf)
	{
		// 写一个整个BUF块
		write_block((void *) buf.data(), buf.size());
	}

	// 移除前面部分数据
	void remove_pos(int len)
	{
		if (len < 0 || _pfree == NULL || _pbuf == NULL)
			return;
		int size = (int) (_pfree - _pbuf);
		if (len > size)
		{
			reset_buf();
			return;
		}

		int left = size - len;
		memmove(_pbuf, _pbuf + len, left);
		_pfree = _pbuf + left;
	}

	// 确保有len的空余空间
	void ensure_free(int len)
	{
		expand(len);
	}
	// 移动处理过数据空闲指针
	void pour_data(int len)
	{
		assert(_pend - _pfree >= len);
		_pfree += len;
	}

	// 取得可用指针
	char *get_free()
	{
		return (char*) _pfree;
	}

	// 取得可用空间大小
	int get_free_len()
	{
		return (_pend - _pfree);
	}

protected:
	// 扩展数据的内存
	inline void expand(int need)
	{
		if (_pbuf == NULL)
		{
			int len = 256;
			while (len < need)
				len <<= 1;
			_pfree = _pdata = _pbuf = (unsigned char*) malloc(len);
			_pend = _pbuf + len;
		}
		else if (_pend - _pfree < need)
		{ // 空间不够
			int flen = _pend - _pfree;  // 可用内存空间
			int dlen = _pfree - _pbuf;	 // 数据空间大小

			if (flen < need || flen * 4 < dlen)
			{
				int bufsize = (_pend - _pbuf) * 2;
				while (bufsize - dlen < need)
					bufsize <<= 1;

				unsigned char *newbuf = (unsigned char *) malloc(bufsize);

				assert(newbuf != NULL);
				if (dlen > 0)
				{
					memcpy(newbuf, _pbuf, dlen);
				}
				free(_pbuf);

				_pdata = _pbuf = newbuf;
				_pfree = _pbuf + dlen;
				_pend = _pbuf + bufsize;
			}
		}
	}

protected:
	// 数据对象
	unsigned char *_pbuf;
	// 数据缓存对象
	unsigned char *_pdata;
	// 数据空间大小
	unsigned char *_pend;
	// 数据使用偏移
	unsigned char *_pfree;
	// 是否为只读的数据
	unsigned char _ronly;
};

} // namespace triones

#endif // #ifndef __TRIONES_DATABUFFER_H__
