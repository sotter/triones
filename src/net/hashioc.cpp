/******************************************************
 *   FileName: hashioc.cpp
 *     Author: triones  2014-11-6
 *Description:
 *******************************************************/

#include "hashioc.h"

bool HashSock::init(size_t capacity, size_t locks)
{
	size_t factor = 4;
	_ht_size = next_power(capacity / (factor < 1 ? 1 : factor), HT_INITIAL_SIZE);
	_ht_mask = _ht_size - 1;

	_lock_size = next_power(locks, LOCK_INITIAL_SIZE);
	_lock_mask = _lock_size - 1;

	_hash_table = new EntryQueue[_ht_size];
	_lock_array = new triones::RWMutex[_lock_size];

	return true;
}

//如果返回失败的话，需要外部将自己的ioc删除, 锁由外面进行显式调用
bool HashSock::put(int64_t sockid, IOComponent *ioc)
{
	if (ioc == NULL) return false;

	bool ret = false;
	unsigned index = sock_hash(sockid);
	EntryQueue &queue = _hash_table[index & _ht_mask];

	//注意：这个地方不检测是否已经存在该ioc,外部加入调用的时候已经先调用了get在get为NULL时
	//且get操作和add操作是在同一把锁当中同步的。
	entry *e = new entry;
	e->_sockid = sockid;
	e->_ioc = ioc;
	queue.push(e);
	ret = true;

	return true;
}

//不提供get出去的接口。
IOComponent* HashSock::get(int64_t sockid)
{
	IOComponent* v = NULL;

	unsigned index = sock_hash(sockid);
	EntryQueue &queue = _hash_table[index & _ht_mask];
	entry *e = queue.begin();
	for (; e != NULL; e = queue.next(e))
	{
		if (e->_sockid == sockid)
		{
			v = e->_ioc;
			break;
		}
	}

	return v;
}

string HashSock::run_info()
{
	string runinfo;
	return runinfo;
}

int HashSock::clear(time_t timeout)
{
	UNUSED(timeout);
	return 0;

//		int cnt = 0;
//
//		for (size_t i = 0; i < _ht_size; i++)
//		{
//			ct_write_lock(i);
//			EntryQueue &queue = _hash_table[i];
//			entry *e = queue.begin();
//			time_t now = time(NULL);
//
//			for (; e != NULL;)
//			{
//				entry *p = e;
//				e = queue.next(e);
//
//				if (p->_value->_vstate == iocfsm::OFF_LINE
//				        && now - p->_value->_last_access_time > timeout)
//				{
//					queue.erase(p);
//					delete p->_value;
//					delete p;
//					cnt++;
//				}
//			}
//			ct_write_unlock(i);
//		}
//
//		return cnt;
}

unsigned int HashSock::sock_hash(int64_t sockid)
{

#define TAB_BITS 8

	struct triones_sockaddr
	{
		unsigned short family;
		unsigned short port;
		unsigned int host;
	};

	union seriaddr
	{
		uint64_t sockid;
		triones_sockaddr sockaddr;
	};

	seriaddr s;
	s.sockid = sockid;

	unsigned porth = ntohs(s.sockaddr.port);

	return (s.sockaddr.family ^ ntohl(s.sockaddr.host) ^ (porth >> TAB_BITS) ^ porth);
}

size_t HashSock::next_power(size_t size, size_t mini)
{
	if (size >= ULONG_MAX) return ULONG_MAX;
	size_t n = mini;

	while (1)
	{
		if (n >= size) return n;
		n = (n << 2);
	}
	return n;
}
