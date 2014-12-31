/*
 * idbaseuser.cpp
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#include "idbaseuser.h"
#include "../net/tbtimeutil.h"

namespace triones
{
IdBaseUser::IdBaseUser()
: _hash_size(HASH_SIZE)
, _hash_mask(HASH_SIZE - 1)
{
	// TODO Auto-generated constructor stub
	_lock_array = new triones::RWMutex[HASH_SIZE];
	_id_user = new IdMap[HASH_SIZE];
	_last_del_time = 0;
}

IdBaseUser::~IdBaseUser() {
	// 从_id_user中删除用户
	for (size_t hash_index = 0; hash_index < _hash_size; ++hash_index)
	{
		ct_write_lock(hash_index);
		for (IdIter it = _id_user->begin(); it != _id_user->end(); )
		{
			BaseUser* tmp = it->second;
			delete tmp;
			_id_user->erase(it++);
		}
		ct_write_unlock(hash_index);
	}

	// 从_del_list中删除用户
	_del_lock.lock();
	for (DelIter it = _del_list.begin(); it != _del_list.end(); )
	{
		BaseUser* tmp = *it;
		delete tmp;

		_del_list.erase(it++);
	}
	_del_lock.unlock();

	// 删除_id_user
	if (_id_user)
	{
		delete[] _id_user;
		_id_user = NULL;
	}

	// 删除锁
	if (_lock_array)
	{
		delete[] _lock_array;
		_lock_array = NULL;
	}
}

// 获取hash桶大小
size_t IdBaseUser::get_hash_size()
{
	return _hash_size;
}

// 获取hash_index
size_t IdBaseUser::get_hash_index(const std::string& id)
{
	unsigned int hash_value = CommHash::istr_hash((const unsigned char *)(id.c_str()), id.length());
	size_t hash_index = hash_value & _hash_mask;

	return hash_index;
}

// 添加用户
bool IdBaseUser::add_user(size_t hash_index, BaseUser* user)
{
	if (user == NULL)
	{
		return false;
	}

	bool ret = false;
	std::string user_id = user->get_user_id();
	IsrtRslt result = _id_user[hash_index].insert(std::pair<std::string, BaseUser*>(user_id, user));
	if (result.second) {
		ret = true;
	}

	return ret;
}

// 使用时注意加锁
BaseUser* IdBaseUser::get_user(size_t hash_index, const std::string& id)
{
	BaseUser *user = NULL;

	std::map<std::string, BaseUser*>::iterator it = _id_user[hash_index].find(id);
	if (it != _id_user[hash_index].end())
	{
		user = it->second;
	}

	return user;
}

// 删除用户
bool IdBaseUser::remove_user(size_t hash_index, const std::string& id)
{
	bool ret = false;

	std::map<std::string, BaseUser*>::iterator it = _id_user[hash_index].find(id);
	if (it != _id_user[hash_index].end())
	{
		// 加入待删除列表
		BaseUser* tmp = it->second;
		_del_lock.lock();
		_del_list.push_back(tmp);
		_del_lock.unlock();

		// 从map中移除用户
		_id_user[hash_index].erase(it);
		ret = true;
	}

	return ret;
}

// 释放user
void IdBaseUser::timer_work(uint64_t now)
{
	if (now - _last_del_time > 5)
	{
		_del_lock.lock();
		for (DelIter it = _del_list.begin(); it != _del_list.end(); )
		{
			// 释放内存
			BaseUser* tmp = *it;
			if (tmp->get_ref() <= 0)
			{
				delete tmp;
			}

			// 从列表中删除指针
			_del_list.erase(it++);
		}
		_del_lock.unlock();

		_last_del_time = triones::CTimeUtil::get_time();
	}
}

// 加读锁
void IdBaseUser::ct_read_lock(size_t hash_index)
{
	_lock_array[hash_index].rdlock();
}

// 解读锁
void IdBaseUser::ct_read_unlock(size_t hash_index)
{
	_lock_array[hash_index].unlock();
}

// 加写锁
void IdBaseUser::ct_write_lock(size_t hash_index)
{
	_lock_array[hash_index].wrlock();
}

// 解写锁
void IdBaseUser::ct_write_unlock(size_t hash_index)
{
	_lock_array[hash_index].unlock();
}

}
