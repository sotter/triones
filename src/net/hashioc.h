/******************************************************
 *   FileName: hashioc.h
 *     Author: triones  2014-11-6
 *Description: 本类是专门提供给tranport使用的，与transport的联系紧密不具通用性
 *******************************************************/

#ifndef HASHIOC_H_
#define HASHIOC_H_

#include "../comm/tqueue.h"

namespace triones
{

const size_t HT_INITIAL_SIZE = 4;
const size_t LOCK_INITIAL_SIZE = 1;

class HashSock
{
public:

	typedef TQueue<IOComponent> IOCQueue;

public:
	HashSock();

	virtual ~HashSock();

	// 初始化认100w/4 = 25w个hash bucket, 128的队列锁
	bool init(Transport *t, size_t capacity = (1024 * 1024), size_t locks = 128);

	// 如果返回失败的话，需要外部将自己的ioc删除, 锁由外面进行显式调用
	bool put(IOComponent *ioc);

	// 根据IOComponent的ID获取IOComponent， 在业务层业务用户的绑定可以绑定sockid, 而不必一定绑定IOComponent本身
	// 这样可以在牺牲很小的性能下，极大的降低了业务层和网络的耦合；但同时注意：如果业务数据不及时处理，sockid被其它人复用，会造成业务上的错乱。
	IOComponent* get(uint64_t sockid);

	//将sockid从列表管理中删除, 放入到recycle队列中
	bool erase(uint64_t sockid);

	//将ioc移除掉，并释放其网络层资源
	IOComponent *remove(IOComponent *ioc);

	//获取需要删除的IOC, IOC的引用计数为0时，将其删除
	void get_del_list(IOCQueue &del);

	// 获取HashSock的管理
	string run_info();

	// 超时检测，清理等；待实现 2014-11-06
	int check_timeout();

	//清空
	void distroy();

private:

	//将ioc放入到recycle或是delete队列
	bool moveto_recycle(IOComponent *ioc);

	//采用IPVS的hash算法，均衡性未经测试, todo: 待测试
	unsigned int sock_hash(uint64_t sockid);

	//获取2的指数倍的数字，mini表示最小值
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
	//是否初始化标志位
	bool _stop;
	//hash桶
	IOCQueue *_hash_table;
	//hash桶个数
	size_t _ht_size;
	//hash桶掩码
	size_t _ht_mask;
	//当前桶中的数目
	int _size;
	//hash桶锁队列
	triones::RWMutex *_lock_array;
	//锁队列的大小
	size_t _lock_size;
	//锁队列掩码
	size_t _lock_mask;
	//提供给recycle队列操作的锁
	triones::Mutex _recycle_lock;
	//回收后但是引用计数不为0的队列
	IOCQueue _recycle;
	//提供给del队列操作的锁
	triones::Mutex _del_lock;
	//回收后等待删除的队列
	IOCQueue _del;
	//传输类
	triones::Transport *_transport;
};

}
#endif /* HASHIOC_H_ */
