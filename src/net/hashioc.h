/******************************************************
 *   FileName: hashioc.h
 *     Author: triones  2014-11-6
 *Description:
 *******************************************************/

#ifndef HASHIOC_H_
#define HASHIOC_H_

#include "cnet.h"
#include "../comm/tqueue.h"

namespace triones
{

const size_t HT_INITIAL_SIZE = 4;
const size_t LOCK_INITIAL_SIZE = 1;

class HashSock
{
public:
	typedef struct _entry
	{
		int64_t _sockid;
		IOComponent *_ioc;
		struct _entry *_pre;
		struct _entry *_next;
	} entry;

	typedef TQueue<entry> EntryQueue;

public:
	// 初始化认100w/4 = 25w个hash bucket, 128的队列锁
	bool init(size_t capacity = (1024 * 1024), size_t locks = 128);

	// 如果返回失败的话，需要外部将自己的ioc删除, 锁由外面进行显式调用
	bool put(int64_t sockid, IOComponent *ioc);

	// 根据IOComponent的ID获取IOComponent， 在业务层业务用户的绑定可以绑定sockid, 而不必一定绑定IOComponent本身
	// 这样可以在牺牲很小的性能下，极大的降低了业务层和网络的耦合；但同时注意：如果业务数据不及时处理，sockid被其它人复用，会造成业务上的错乱。
	IOComponent* get(int64_t sockid);

	// 获取HashSock的管理
	string run_info();

	// 超时检测，清理等；待实现 2014-11-06
	int clear(time_t timeout);

private:

	unsigned int sock_hash(int64_t sockid);

	size_t next_power(size_t size, size_t mini);

	void ct_read_lock(unsigned int index)
	{
		_lock_array[index & _lock_mask].rdlock();
	}

	void ct_read_unlock(unsigned int index)
	{
		_lock_array[index & _lock_mask].unlock();
	}

	void ct_write_lock(unsigned int index)
	{
		_lock_array[index & _lock_mask].wrlock();
	}

	void ct_write_unlock(unsigned int index)
	{
		_lock_array[index & _lock_mask].unlock();
	}

private:
	bool _inited;
	EntryQueue *_hash_table;
	size_t _ht_size;
	size_t _ht_mask;

	triones::RWMutex *_lock_array;
	size_t _lock_size;
	size_t _lock_mask;
};

}
#endif /* HASHIOC_H_ */
