/******************************************************
 *   FileName: hashioc.cpp
 *     Author: triones  2014-11-6
 *Description:
 *******************************************************/

#include "cnet.h"
#include <limits.h>


HashSock::HashSock()
{
	_transport = NULL;
	_stop = false;
}

HashSock::~HashSock()
{
	if(! _stop)
	{
		distroy();
	}
}

bool HashSock::init(Transport *t, size_t capacity, size_t locks)
{
	_transport = t;

	size_t factor = 4;
	_ht_size = next_power(capacity / (factor < 1 ? 1 : factor), HT_INITIAL_SIZE);
	_ht_mask = _ht_size - 1;

	_lock_size = next_power(locks, LOCK_INITIAL_SIZE);
	_lock_mask = _lock_size - 1;

	_hash_table = new IOCQueue[_ht_size];
	_lock_array = new triones::RWMutex[_lock_size];

	return true;
}

//如果返回失败的话，需要外部将自己的ioc删除, 锁由外面进行显式调用
bool HashSock::put(IOComponent *ioc)
{
	if (_stop || ioc == NULL || ioc->getid() == 0) return false;

	unsigned index = sock_hash(ioc->getid());
	IOCQueue &queue = _hash_table[index & _ht_mask];

	//是否已经有这个sockid的数据，暂时不做检查，由业务层保证sockid的唯一性
	ct_write_lock(index);
	printf("==================hashsock put %lu type %d ==============\n", ioc->getid(), ioc->get_type());
	queue.push(ioc);
	_size++;
	ct_write_unlock(index);

	return true;
}

//不提供get出去的接口。
IOComponent* HashSock::get(uint64_t sockid)
{
	if(_stop) return NULL;

	IOComponent* v = NULL;

	unsigned index = sock_hash(sockid);
	IOCQueue &queue = _hash_table[index & _ht_mask];

	ct_read_lock(index);
	IOComponent *e = queue.begin();
	for (; e != NULL; e = queue.next(e))
	{
		if (e->getid() == sockid)
		{
			v = e;
			break;
		}
	}
	ct_read_unlock(index);

	return v;
}

//将sockid从列表管理中删除, 放入到recycle队列中
bool HashSock::erase(uint64_t sockid)
{
	if(_stop) return false;

//	IOComponent* v = NULL;
	unsigned index = sock_hash(sockid);

	ct_read_lock(index);
	IOCQueue &queue = _hash_table[index & _ht_mask];
	IOComponent *e = queue.begin();
	for (; e != NULL; e = queue.next(e))
	{
		if (e->getid() == sockid)
		{
			break;
		}
	}
	ct_read_unlock(index);

	if(e == NULL)
		return false;

	//从队列总将其删除
	ct_write_lock(index);
	queue.erase(e);
	_size--;
	ct_write_unlock(index);

	//放入到回收队列
	moveto_recycle(e);

	return true;
}

//暂未实现
IOComponent *HashSock::remove(IOComponent *ioc)
{
	if(_stop || ioc == NULL) return NULL;

	unsigned index = sock_hash(ioc->getid());

	ct_write_lock(index);
	IOCQueue &queue = _hash_table[index & _ht_mask];
	queue.erase(ioc);
	_size--;
	ct_write_unlock(index);

	return ioc;
}

//获取需要删除的IOC, IOC的引用计数为0时，将其删除
void HashSock::get_del_list(IOCQueue &del)
{
	if(_stop) return;

	//先把要删除的移送到del中
	_del_lock.lock();
	_del.moveto(del);
	_del_lock.unlock();

	//检查_recycle队列的引用计数
	_recycle_lock.lock();
	IOComponent *e = _recycle.begin();
//	for (; e != NULL; e = _recycle.next(e))
	while(e != NULL)
	{
		//检查引用计数是否为0， 如果为0将这条数据放入到del中， 将这条数据放入到del中
		if (e->get_ref() == 0)
		{
			IOComponent *cur = e->_next;
			_recycle.erase(e);
			del.push(e);
			e = cur;
		}
		else
		{
			e = _recycle.next(e);
		}
	}
	_recycle_lock.unlock();

	return;
}

string HashSock::run_info()
{
	string runinfo;
	return runinfo;
}

int HashSock::check_timeout()
{
	if(_stop) return 0;

	uint64_t now = triones::CTimeUtil::get_time();

	IOCQueue timeoutlist;
	for(size_t i = 0; i < _ht_size; i++)
	{
		ct_write_lock(i);
		IOCQueue &queue = _hash_table[i];
		IOComponent *e = queue.begin();
		while (e != NULL)
		{
			//如果检测到超时了，将其放入到超时队列中
			if (e->check_timeout(now))
			{
				IOComponent *cur = e->_next;
				queue.erase(e);
				timeoutlist.push(e);
				e = cur;
			}
			else
			{
				e = queue.next(e);
			}
		}
		ct_write_unlock(i);
	}

	IOComponent *e = NULL;
	while((e = timeoutlist.pop()) != NULL)
	{
		//回调这个关闭掉
		e->close();
		moveto_recycle(e);
	}

	return 0;
}

//清空
void HashSock::distroy()
{
	_stop = true;
	//不管引用计数是否为0，全部删除
	IOComponent *e = NULL;
	for (size_t i = 0; i < _ht_size; i++)
	{
		ct_write_lock(i);
		IOCQueue &queue = _hash_table[i];

		while ((e = queue.pop()) != NULL)
		{
			delete e;
		}

		ct_write_unlock(i);
	}

	_recycle_lock.lock();
	while((e = _recycle.pop()) != NULL)
	{
		delete e;
	}
	_recycle_lock.unlock();

	_del_lock.lock();
	while ((e = _del.pop()) != NULL)
	{
		delete e;
	}
	_del_lock.unlock();

	return;
}

//将ioc放入到recycle或是delete队列
bool HashSock::moveto_recycle(IOComponent *ioc)
{
	if (ioc->get_ref() <= 0)
	{
		_del_lock.lock();
		_del.push(ioc);
		_del_lock.unlock();
	}
	else
	{
		_recycle_lock.lock();
		_recycle.push(ioc);
		_recycle_lock.unlock();
	}

	return true;
}

//采用IPVS的hash算法，均衡性未经测试
unsigned int HashSock::sock_hash(uint64_t sockid)
{
#define TAB_BITS 8

	unsigned port = htons(sockid >> 32);
	unsigned family = (sockid >> 48);
	unsigned host = sockid;

	return (family ^ ntohl(host) ^ (port >> TAB_BITS) ^ port);
}

size_t HashSock::next_power(size_t size, size_t mini)
{
	if (size >= ULONG_MAX) return ULONG_MAX;
	size_t n = mini;

	while (1)
	{
		if (n >= size)
			return n;
		n = (n << 2);
	}
	return n;
}
