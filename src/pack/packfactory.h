/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_PACKFACTORY_H__
#define __TRIONES_PACKFACTORY_H__

#include "pack.h"
#include "mutex.h"

namespace triones
{

// 序列产生器
class CSequeueGen
{
public:
	CSequeueGen():_seq(0) {};
	~CSequeueGen(){};
	// 取得序号
	unsigned int GetSequeue( void ) {
		Guard guard(_mutex) ;
		return ++_seq ;
	}

private:
	// 同步操作锁
	Mutex  _mutex ;
	// 序列对象
	unsigned int  _seq ;
};

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
	virtual MsgPacket* unpack(unsigned short msgtype, Packet &pack) = 0;
};

class PackFactory
{
public:
	PackFactory(IUnpackMgr* packmgr)
	{
		_packmgr = packmgr;
	}

	MsgPacket* unpack(Packet* pack)
	{
		if (pack == NULL) {
			return NULL;
		}

		// 基本长度检查
		if (pack->getDataLen() < (int)sizeof(CMsgHeader)) {
			return NULL;
		}

		// 验证版本号
		unsigned short msg_ver = pack->readInt16();
		if (msg_ver != MSG_VERSION) {
			return NULL;
		}

		// 获取数据类型
		unsigned short msg_type = pack->readInt16();

		// 解包
		pack->seekPos(0);
		MsgPacket* msg_pack = _packmgr->unpack(msg_type, *pack);

		return msg_pack;
	}

	// 取得序号
	unsigned int GetSequeue(void)
	{
		return _seqgen.GetSequeue();
	}
private:
	// 数据解包对象
	IUnpackMgr* _packmgr;
	// 序号产生对象
	CSequeueGen _seqgen ;
};

} // namespace triones

#endif // #ifndef __TRIONES_PACKFACTORY_H__
