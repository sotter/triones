/*
 * userjober.h
 *
 *  Created on: 2015年1月23日
 *      Author: water
 */

#ifndef USERJOBER_H_
#define USERJOBER_H_
#include "stddef.h"

namespace triones
{

class BaseUser;

class IUserJober
{
public:
	// 虚析构函数
	virtual ~IUserJober(){};
	// 函数对象接口
	virtual void operator()(uint64_t now, BaseUser *user){}
};

// 函数模板，可用于保存任何拥有此签名的成员函数
template<typename CLASS_NAME>
class UserJober: public IUserJober
{
public:
	typedef void (CLASS_NAME::*Func_t)(uint64_t now, BaseUser *user);
public:
	UserJober(CLASS_NAME* class_pointer, Func_t member_func)
	: _class_pointer(class_pointer)
	, _member_func(member_func)
	{}

	// 函数对象接口
	virtual void operator()(uint64_t now, BaseUser *user)
	{
		(_class_pointer->*_member_func)(now, user);
	}
private:
	// 类指针
	CLASS_NAME* _class_pointer;
	// 类成员函数
	Func_t _member_func;
};

class JoberHolder: public IUserJober
{
public:
	// _jober初始化NULL
	JoberHolder(): _jober(NULL) {}

	// 类退出时销毁资源
	~JoberHolder()
	{
		if (_jober) {
			delete _jober;
			_jober = NULL;
		}
	}

	// 只要成员函数接口一致，可用于保存任何类的成员函数
	template<typename CLASS_NAME>
	void set(CLASS_NAME* class_pointer, void (CLASS_NAME::*member_func)(uint64_t, BaseUser*))
	{
		// 可以多次设置,以达到重用Holder的目的
		if (_jober) {
			delete _jober;
			_jober = NULL;
		}

		_jober = new UserJober<CLASS_NAME>(class_pointer, member_func);
	}

	// 函数对象接口
	virtual void operator()(uint64_t now, BaseUser *user)
	{
		if (_jober) {
			(*_jober)(now, user);
		}
	}
private:
	IUserJober* _jober;
};
}


#endif /* USERJOBER_H_ */
