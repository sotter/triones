/*
 * iduser.h
 *
 *  Created on: 2014年12月18日
 *      Author: water
 */

#ifndef IDUSER_H_
#define IDUSER_H_
#include "user/idbaseuser.h"

namespace triones
{

class TransProtocol;

class IdUser: public IdBaseUser
{
public:
	IdUser();
	virtual ~IdUser();

	// 群发消息
	bool group_send(const std::string& xml, const std::string& from_user);

	void set_trans_proto(TransProtocol* tp)
	{
		_tp = tp;
	}

private:
	TransProtocol *_tp;
};

} /* namespace triones */

#endif /* IDUSER_H_ */
