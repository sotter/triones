/*
 * baseuser.h
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#ifndef BASEUSER_H_
#define BASEUSER_H_
#include <map>
#include <string>
#include "../net/atomic.h"

namespace triones
{
class IOComponent;

class BaseUser {
public:
	BaseUser();
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

	void set_ioc(IOComponent* ioc)
	{
		_ioc = ioc;
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
protected:
	// 用户id
	std::string _user_id;

	// ioc指针
	IOComponent *_ioc;

	// 引用计数
	atomic_t _refcount;
};
}

#endif /* BASEUSER_H_ */
