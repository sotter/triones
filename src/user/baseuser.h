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

protected:
	std::string _user_id;
	IOComponent *_ioc;
};
}

#endif /* BASEUSER_H_ */
