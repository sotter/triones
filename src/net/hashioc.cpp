/******************************************************
 *   FileName: hashioc.cpp
 *     Author: triones  2014-11-6
 *Description:
 *******************************************************/

#include "cnet.h"
#include <limits.h>

HashSock::HashSock(Transport *t, size_t capacity, size_t locks)
{
	_transport = NULL;
	_stop = false;

	_transport = t;
	size_t factor = 4;
	_ht_size = next_power(capacity / (factor < 1 ? 1 : factor), HT_INITIAL_SIZE);
	_ht_mask = _ht_size - 1;

	_lock_size = next_power(locks, LOCK_INITIAL_SIZE);
	_lock_mask = _lock_size - 1;

	_hash_table = new SetQueue[_ht_size];
	_lock_array = new triones::RWMutex[_lock_size];
}

HashSock::~HashSock()
{
	if(! _stop)
	{
		distroy();
	}
}

//如果返回失败的话，需要外部将自己的ioc删除, 锁由外面进行显式调用
bool HashSock::put(IOComponent *ioc)
{
	if (_stop || ioc == NULL || ioc->getid() == 0) return false;

	unsigned sock_index = sock_hash(ioc->getid());
	unsigned hash_index = sock_index & _ht_mask;
	IOCQueue &queue = _hash_table[hash_index]._queue;
	IocSet &set = _hash_table[hash_index]._set;


	bool result = false;

	{
		ct_write_lock(hash_index);
		SetResult ret = set.insert(ioc);
		if (ret.second)
		{
			printf("==================hashsock put %lu type %d ==============\n", ioc->getid(), ioc->get_type());
			queue.push(ioc);
			//_size++;
			add_size();
			result = true;
		}
		ct_write_unlock(hash_index);
	}

	return result;
}

//不提供get出去的接口。
IOComponent* HashSock::get(uint64_t sockid)
{
	if(_stop) return NULL;

	IOComponent* v = NULL;

	unsigned index = sock_hash(sockid);
	IOCQueue &queue = _hash_table[index & _ht_mask]._queue;

	{
		ct_read_lock(index);
		for (v = queue.end(); v != NULL; v = queue.pre(v))
		{
			if (v->getid() == sockid)
			{
				break;
			}
		}
		ct_read_unlock(index);
	}

	return v;
}

//将sockid从列表管理中删除, 放入到recycle队列中
bool HashSock::erase(uint64_t sockid)
{
	if(_stop) return false;

//	IOComponent* v = NULL;
	unsigned sock_index = sock_hash(sockid);
	unsigned hash_index = sock_index & _ht_mask;

	// 是否查找成功
	bool found = false;
	IOComponent* e = NULL;
	{
		ct_read_lock(hash_index);
		IOCQueue &queue = _hash_table[hash_index]._queue;
		for (e = queue.begin(); e != NULL; e = queue.next(e))
		{
			if (e->getid() == sockid)
			{
				found = true;
				break;
			}
		}
		ct_read_unlock(hash_index);
	}

	// 未找到，删除失败
	if (!found)
	{
		return false;
	}

	// 删除是否成功
	bool del_result = false;
	{
		ct_write_lock(hash_index);
		IocSet &set = _hash_table[hash_index]._set;
		IOCQueue &queue = _hash_table[hash_index]._queue;
		SetIter iter = set.find(e);
		if (iter != set.end())
		{
			set.erase(e);

			//从队列中将其删除
			queue.erase(e);
			//_size--;
			sub_size();

			//放入到回收队列
			moveto_recycle(e);

			del_result = true;
		}
		ct_write_unlock(hash_index);
	}

	// 返回删除结果
	return del_result;
}

IOComponent *HashSock::remove(IOComponent *ioc)
{
	if(_stop || ioc == NULL) return NULL;

	unsigned sock_index = sock_hash(ioc->getid());
	unsigned hash_index = sock_index & _ht_mask;

	// 是否删除成功
	bool del_ret = false;
	{
		ct_write_lock(hash_index);
		IocSet &set = _hash_table[hash_index]._set;
		IOCQueue &queue = _hash_table[hash_index]._queue;

		SetIter iter = set.find(ioc);
		if (iter != set.end())
		{
			set.erase(iter);

			queue.erase(ioc);
			//_size--;
			sub_size();

			del_ret = true;
		}
		ct_write_unlock(hash_index);
	}

	if (del_ret)
	{
		_recycle_lock.lock();
		_recycle.push(ioc);
		_recycle_lock.unlock();
	}

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

int HashSock::get_timeout_list(IOCQueue &timeoutlist)
{
	if(_stop) return 0;

	uint64_t now = triones::CTimeUtil::get_time();

	for(size_t i = 0; i < _ht_size; i++)
	{
		ct_write_lock(i);
		IOCQueue &queue = _hash_table[i]._queue;
		IocSet &set = _hash_table[i]._set;
		IOComponent *e = queue.begin();
		while (e != NULL)
		{
			//如果检测到超时了，将其放入到超时队列中
			if (e->check_timeout(now))
			{
				IOComponent *cur = e->_next;
				{
					SetIter iter = set.find(e);
					if (iter != set.end())
					{
						set.erase(iter);

						queue.erase(e);
						//_size--;
						sub_size();

						timeoutlist.push(e);
					}
				}
				e = cur;
			}
			else
			{
				e = queue.next(e);
			}
		}
		ct_write_unlock(i);
	}

//	IOComponent *e = NULL;
//	while((e = timeoutlist.pop()) != NULL)
//	{
//		//回调这个关闭掉
//		//e->close();
//		moveto_recycle(e);
//	}

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
		IOCQueue &queue = _hash_table[i]._queue;
		IocSet &set = _hash_table[i]._set;

		set.clear();
		while ((e = queue.pop()) != NULL)
		{
			printf("delete %lu \n", e->getid());
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

	if(_hash_table != NULL)
	{
		delete []_hash_table;
		_hash_table = NULL;
	}

	if(_lock_array != NULL)
	{
		delete []_lock_array;
		_lock_array = NULL;
	}

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
