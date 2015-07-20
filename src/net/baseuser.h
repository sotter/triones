/*
 * baseuser.h
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#ifndef BASEUSER_H_
#define BASEUSER_H_

//#include <map>
#include <string>
#include "../net/atomic.h"

namespace triones
{

enum
{
	USER_OFF_LINE = 0, 	// 下线状态
	USER_CONNECTING, 	// 发起连接，但尚未连接成功状态
	USER_WAIT_RESP, 	// 发起登录操作，但尚未登录成功状态
	USER_ONLINE, 		// 登录成功状态
	DISABLED			// 禁用状态
};

//class UserScanHandler
//{
//public:
//	virtual ~UserScanHandler(){}
//
//	//user遍历时的回调函数
//	virtual void user_scan_handler(BaseUser *user, uint64_t now, void *param) = 0;
//};

class BaseUser
{
public:

	BaseUser()
	: _user_state(USER_OFF_LINE)
	, _ioc(NULL)
	, _heart_beat_span(30)
	, _last_active_time(0)
	, _hash_index(-1)
	{
		atomic_set(&_refcount, 0);
	};

	virtual ~BaseUser();

	std::string get_user_id()
	{
		return _user_id;
	}

	void set_user_id(const std::string& user_id)
	{
		_user_id = user_id;
	}

	IOComponent* get_ioc()
	{
		return _ioc;
	}

	void set_state(int s)
	{
		_user_state = s;
	}

	int get_state()
	{
		return _user_state;
	}

	void bind_ioc(IOComponent *ioc)
	{
		if(_ioc != NULL)
		{
			unbind_ioc();
		}

		if(ioc != NULL)
		{
			ioc->add_ref();
			_ioc = ioc;
		}
	}

	void unbind_ioc()
	{
		if(_ioc != NULL)
			_ioc->sub_ref();
		_ioc = NULL;
	}

	int add_ref()
	{
		return atomic_add_return(1, &_refcount);
	}

	void sub_ref()
	{
		atomic_dec(&_refcount);
	}

	int get_ref()
	{
		return atomic_read(&_refcount);
	}

	bool ref_none()
	{
		return atomic_read(&_refcount) <= 0;
	}

	void set_conn_addr(const std::string& conn_addr)
	{
		_conn_addr = conn_addr;
	}

	std::string get_conn_addr()
	{
		return _conn_addr;
	}

	void set_user_name(const std::string& user_name)
	{
		_user_name = user_name;
	}

	std::string get_user_name()
	{
		return _user_name;
	}

	void set_user_pwd(const std::string& user_pwd)
	{
		_user_pwd = user_pwd;
	}

	std::string get_user_pwd()
	{
		return _user_pwd;
	}

	void set_last_active_time(uint64_t last_active_time)
	{
		_last_active_time = last_active_time;
	}

	uint64_t get_last_active_time()
	{
		return _last_active_time;
	}

	void set_heart_beat_span(int heart_beat_span)
	{
		_heart_beat_span = heart_beat_span;
	}

	int get_heart_beat_span()
	{
		return _heart_beat_span;
	}

	void set_hash_index(int hash_index)
	{
		_hash_index = hash_index;
	}

	int get_hash_index()
	{
		return _hash_index;
	}
//protected:
public:

	// 用户id
	std::string _user_id;

	//用户状态
	int _user_state;

	// ioc指针
	IOComponent *_ioc;

	// 引用计数
	atomic_t _refcount;

	// 连接地址，如tcp:127.0.0.1:80、udp:127.0.0.1:2349
	std::string _conn_addr;

	// 用户名
	std::string _user_name;

	// 密码
	std::string _user_pwd;

	// 最后活跃时间，用于判断是否应该发送心跳数据包
	uint64_t _last_active_time;

	// 当网络空闲时，应该每隔多久发送一次心跳数据包
	int _heart_beat_span;

	// User所属的hash列表的索引
	int _hash_index;
};

class UserHolder
{
public:
	UserHolder(): _user(NULL) {}

	~UserHolder()
	{
		if (_user)
		{
			_user->sub_ref();
			_user = NULL;
		}
	}

	void set_user(BaseUser* user)
	{
		_user = user;
		_user->add_ref();
	}

private:
	BaseUser* _user;
};

class IUserFactory
{
public:
	virtual ~IUserFactory(){}
	virtual BaseUser* create_user(int user_type) = 0;
};
}

#endif /* BASEUSER_H_ */
