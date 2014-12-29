/*
 * user.h
 *
 *  Created on: 2014年12月18日
 *      Author: water
 */

#ifndef USER_H_
#define USER_H_
#include "user/baseuser.h"

namespace triones {

class User : public BaseUser {
public:
	User();
	virtual ~User();
};

} /* namespace triones */

#endif /* USER_H_ */
