/*
 * IdUser.cpp
 *
 *  Created on: 2014年12月18日
 *      Author: water
 */

#include "iduser.h"
#include "pack/pack.h"
#include "net/cnet.h"
#include "net/iocomponent.h"

namespace triones {

IdUser::IdUser() {
	// TODO Auto-generated constructor stub

}

IdUser::~IdUser() {
	// TODO Auto-generated destructor stub
}

// 群发消息
bool IdUser::group_send(const std::string& xml, const std::string& from_user)
{
	bool ret = false;
	for (size_t index = 0; index < _hash_size; index++) {
		ct_read_lock(index);
		for (IdIter it = _id_user[index].begin(); it != _id_user[index].end(); ++it) {
			std::string user_id = it->second->get_user_id();
			if (user_id.empty()) {
				continue;
			}

			IOComponent* ioc = NULL;
			if (user_id != from_user) {
				ioc = it->second->get_ioc();
				if (ioc) {
					Packet* pack = _tp->encode_pack(xml.c_str(), xml.length());
					if (!ioc->post_packet(pack)) {
						delete pack;
					} else {
						ret = true;
					}
				}
			}
		}
		ct_read_unlock(index);
	}


	return ret;
}
} /* namespace triones */
