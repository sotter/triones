/*
 * iduser.h
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#ifndef IDBASEUSER_H_
#define IDBASEUSER_H_
#include <map>
#include <list>

#include "../comm/mutex.h"
#include "../comm/commhash.h"
#include "baseuser.h"
#include "../net/itimerwork.h"

namespace triones
{
class IdBaseUser : public ITimerWork {
protected:
	typedef std::map<std::string, BaseUser*> IdMap;
	typedef std::map<std::string, BaseUser*>::iterator IdIter;
	typedef std::pair<IdIter, bool> IsrtRslt;
	typedef std::list<BaseUser*>::iterator DelIter;
public:
	IdBaseUser();
	virtual ~IdBaseUser();

	// 获取hash桶大小
	size_t get_hash_size();

	// 获取hash_index
	size_t get_hash_index(const std::string& id);

	// 使用时注意加锁
	BaseUser* get_user(size_t hash_index, const std::string& id);

	// 添加用户
	bool add_user(size_t hash_index, BaseUser* user);

	// 移除用户
	bool remove_user(size_t hash_index, const std::string& id);

	// 加读锁
	void ct_read_lock(size_t hash_index);

	// 解读锁
	void ct_read_unlock(size_t hash_index);

	// 加写锁
	void ct_write_lock(size_t hash_index);

	// 解写锁
	void ct_write_unlock(size_t hash_index);

	// 释放user
	virtual void timer_work(uint64_t now);
protected:
	enum {HASH_SIZE = 128};

	// hash桶大小
	size_t _hash_size;

	// 辅助掩码
	size_t _hash_mask;

	// hash桶对应锁
	triones::RWMutex *_lock_array;

	// map: user_id=>user
	IdMap *_id_user;

	// 删除列表锁
	triones::Mutex _del_lock;

	// 上次删除时间
	uint64_t _last_del_time;

	// 待删除列表
	std::list<BaseUser*> _del_list;
};


}

#endif /* IDBASEUSER_H_ */
