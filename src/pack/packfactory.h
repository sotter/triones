/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_PACKFACTORY_H__
#define __TRIONES_PACKFACTORY_H__

#include "pack.h"

namespace triones
{

// 通用解析数据模板类
template<typename T>
MsgPacket *unpack(Packet &pack, const char */*name*/)
{
	T *req = new T;
	if (!req->UnPack(&pack))
	{
		delete req;
		return NULL;
	}

	req->add_ref();
	return req;
}

// 解析协议对象类
class IUnpackMgr
{
public:
	virtual ~IUnpackMgr()
	{
	}
	;

	// 实现数据解包接口方法
	virtual IPacket* unpack(unsigned short msgtype, Packet &pack) = 0;
};

} // namespace triones

#endif // #ifndef __TRIONES_PACKFACTORY_H__
