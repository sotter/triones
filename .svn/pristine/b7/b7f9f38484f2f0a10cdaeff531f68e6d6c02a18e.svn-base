/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_COMMPROTO_H__
#define __TRIONES_COMMPROTO_H__

#include <string>
#include <list>
#include <set>
#include "pack.h"
#include "msgpack.h"
#include "packfactory.h"

namespace triones
{
//////////////////////////////////////////////////////////////////////////////////
// 消息类型
// 共用
const uint16_t LOGIN_REQ                       = 0x1001; // 登录请求
const uint16_t LOGIN_RSP                       = 0x8001; // 登录应答
const uint16_t ACTIVE_REQ                      = 0x1002; // 链路检测请求
const uint16_t ACTIVE_RSP                      = 0x8002; // 链路检测应答
const uint16_t SRVLIST_REQ                     = 0x1003; // 查询服务列表请求
const uint16_t SRVLIST_RSP                     = 0x8003; // 查询服务列表请求应答
const uint16_t SRVLIST_MOD                     = 0x1004; // 服务列表变更通知
const uint16_t SRVLIST_MOD_RSP                 = 0x8004; // 服务列表变更应答

// 基本信息
const uint16_t VEHICLE_INFO_REQ                = 0x1011; // 车辆基本信息查询
const uint16_t VEHICLE_INFO_RSP                = 0x8011; // 车辆基本信息查询应答
const uint16_t VEHICLE_INFO_MOD                = 0x1012; // 车辆基本信息变更通知
const uint16_t VEHICLE_INFO_MOD_RSP            = 0x8012; // 车辆基本信息变更应答
const uint16_t OEM_TERM_INFO_REQ               = 0x1013; // oemcode 跟车机终端类型对应关系查询
const uint16_t OEM_TERM_INFO_RSP               = 0x8013; // oemcode 跟车机终端类型对应关系查询应答
const uint16_t OEM_TERM_INFO_MOD               = 0x1014; // oemcode 跟车机终端类型对应关系更改通知
const uint16_t OEM_TERM_INFO_MOD_RSP           = 0x8014; // oemcode 跟车机终端类型对应关系更改应答
const uint16_t OEM_REG_INFO_REQ                = 0x1015; //	oemcode 跟开户注册码对应关系查询
const uint16_t OEM_REG_INFO_RSP                = 0x8015; //	oemcode 跟开户注册码对应关系查询应答
const uint16_t OEM_REG_INFO_MOD                = 0x1016; //	oemcode 跟开户注册码对应关系更改通知
const uint16_t OEM_REG_INFO_MOD_RSP            = 0x8016; //	oemcode 跟开户注册码对应关系更改应答

// 前置机相关
const uint16_t WAS_REG_CHECK                   = 0x1021; // WAS注册检测
const uint16_t WAS_REG_CHECK_RSP               = 0x8021; // WAS注册检测应答
const uint16_t WAS_AUTH_CHECK                  = 0x1022; // WAS鉴权检测
const uint16_t WAS_AUTH_CHECK_RSP              = 0x8022; // WAS鉴权检测应答
const uint16_t WAS_DRIVER_CHECK                = 0x1023; // 驾驶员信息检测
const uint16_t WAS_DRIVER_CHECK_RSP            = 0x8023; // 驾驶员信息检测应答
const uint16_t WAS_LOGOUT                      = 0x1024; // WAS注销
const uint16_t WAS_LOGOUT_RSP                  = 0x8024; // WAS注销应答
const uint16_t WAS_VEHICLE_INFO_MOD            = 0x1025; // WAS车机数据变更通知
const uint16_t WAS_VEHICLE_INFO_MOD_RSP        = 0x8025; // WAS车机数据变更应答
const uint16_t WAS_BLACK_MOD                   = 0x1026; // 黑名单通知
const uint16_t WAS_BLACK_MOD_RSP               = 0x8026; // 黑名单通知应答

// 透传相关
const uint16_t DP_TERM_AUTH_REQ                = 0x1033; // dp_term车机第三方鉴权码查询
const uint16_t DP_TERM_AUTH_RSP                = 0x8033; // dp_term车机第三方鉴权码查询应答
const uint16_t DP_TERM_AUTH_MOD                = 0x1034; // dp_term第三方鉴权码修改通知
const uint16_t DP_TERM_AUTH_MOD_RSP            = 0x8034; // dp_term第三方鉴权码修改应答
const uint16_t DP_CORP_INFO_REQ                = 0x1035; // dp_term企业信息查询
const uint16_t DP_CORP_INFO_RSP                = 0x8035; // dp_term企业信息查询应答
const uint16_t DP_CORP_INFO_MOD                = 0x1036; // dp_term企业信息更改通知
const uint16_t DP_CORP_INFO_MOD_RSP            = 0x8036; // dp_term企业信息更改应答
const uint16_t DP_CORP_INFO_ALL_REQ            = 0x1037; // dp_term企业信息列表查询请求
const uint16_t DP_CORP_INFO_ALL_RSP            = 0x8037; // dp_term企业信息列表查询请求应答
const uint16_t DP_TERM_CORP_REQ                = 0x1038; // dp_term 跟企业对应关系查询请求
const uint16_t DP_TERM_CORP_RSP                = 0x8038; // dp_term 跟企业对应关系查询请求应答
const uint16_t DP_TERM_CORP_MOD                = 0x1039; // dp_term 跟企业对应关系更改通知
const uint16_t DP_TERM_CORP_MOD_RSP            = 0x8039; // dp_term 跟企业对应关系更改应答
const uint16_t DP_SUBS_REQ                     = 0x1041; // 增加车辆订阅
const uint16_t DP_SUBS_RSP                     = 0x8041; // 增加车辆订阅应答
const uint16_t DP_TRANSFER_REQ                 = 0x1042; // 数据透传
const uint16_t DP_TRANSFER_RSP                 = 0x8042; // 数据透传应答

// PCC 相关
const uint16_t PCC_VEHICLE_INFO_REQ            = 0x1051; // pcc车机数据查询
const uint16_t PCC_VEHICLE_INFO_RSP            = 0x8051; // pcc车机数据查询应答
const uint16_t PCC_VEHICLE_INFO_MOD            = 0x1052; // pcc车机数据变更通知（手机号为 KEY）
const uint16_t PCC_VEHICLE_INFO_MOD_RSP        = 0x8052; // pcc车机数据变更应答（手机号为 KEY）

// Ptrans(协议翻译) Was
const uint16_t PTRANS_FORWARD_UP_REQ            = 0x1062; // was-->ptrans 808上行数据
const uint16_t PTRANS_FORWARD_UP_RSP            = 0x8062; // ptrnas-->was 808上行数据应答

const uint16_t PTRANS_FORWARD_DOWN_REQ          = 0x1063; // ptrans-->was 808下行数据
const uint16_t PTRANS_FORWARD_DOWN_RSP          = 0x8063; // was-->ptrans 808下行数据应答

const uint16_t PTRANS_FORWARD_DISCONNECT_REQ    = 0x1064; // ptrans-->was 车机与was断开(断线)
const uint16_t PTRANS_FORWARD_DISCONNECT_RSP    = 0x8064; // was-->ptrans 数据应答,车与was断开(断线)

const uint16_t PTRANS_FORWARD_CONNECT_REQ       = 0x1065; // ptrans-->was 车机与was连接(上线)
const uint16_t PTRANS_FORWARD_CONNECT_RSP       = 0x8065; // was-->ptrans 车机与was(上线)


const uint8_t  FORWARD_UP_CMD         = 0x01;
const uint8_t  FORWARD_DISCONNECT_CMD = 0x02;//断线
const uint8_t  FORWARD_CONNECT_CMD    = 0x03;//上线


// 登录用户类型
const uint32_t LOGIN_TYPE_DC        = 0x00000001;
const uint32_t LOGIN_TYPE_DS        = 0x00000002;
const uint32_t LOGIN_TYPE_WAS       = 0x00000004;
const uint32_t LOGIN_TYPE_DPMSG     = 0x00000008;
const uint32_t LOGIN_TYPE_DPTERM    = 0x00000010;
const uint32_t LOGIN_TYPE_PCC       = 0x00000020;
const uint32_t LOGIN_TYPE_SUBDPTERM = 0x00000040;
const uint32_t LOGIN_TYPE_MSG       = 0x00000080;
const uint32_t LOGIN_TYPE_SENDSRV   = 0x00000100;
const uint32_t LOGIN_TYPE_PTRANS    = 0x00000200;
const uint32_t LOGIN_TYPE_ALL       = 0xffffffff;


const std::string REDIS_LBS_PHONE       ("lbs.phone");
const std::string REDIS_LBS_CAR         ("lbs.car");
const std::string REDIS_LBS_TERMINAL    ("lbs.terminal");
const std::string REDIS_LBS_REGCODE     ("lbs.regcode");
const std::string REDIS_LBS_BLACKLIST   ("lbs.blacklist");
const std::string REDIS_LBS_DP_CORP     ("lbs.dp.corp");
const std::string REDIS_LBS_DP_TERM_    ("lbs.dp.term.");
const std::string REDIS_LBS_DP_AUTHCODE ("lbs.dp.authcode");
const std::string REDIS_LBS_LOCAL_SRVLIST_("lbs.local.srvlist.");

//////////////////////////////////////////////////////////////////////////////////
// MARK: 共用
class LoginReq: public MsgPacket
{
public:
	LoginReq(uint32_t seq = 0)
	{
		_header._type = LOGIN_REQ;
		_header._seq = seq;
		_type = 0;
		_port = 0;
	}

	virtual ~LoginReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int32(_type);
		pack->write_str(_uid);
		pack->write_str(_ip);
		pack->write_int16(_port);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int32(_type);
		pack->read_str(_uid);
		pack->read_str(_ip);
		pack->read_int16(_port);
		return true;
	}

public:
	uint32_t _type;  // 登录用户类型
	std::string _uid;   // 登陆用户ID，NOTE: 长度最大为 255 字节。
	std::string _ip;    // 登陆用户对外提供服务的 IP，NOTE: 长度最大为 255 字节。
	uint16_t _port;  // 登陆用户对外提供服务的端口
};

class LoginRsp: public MsgPacket
{
public:
	LoginRsp(uint32_t seq = 0)
	{
		_header._type = LOGIN_RSP;
		_header._seq = seq;
		_result = 0;
	}

	virtual ~LoginRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;
};

class ActiveReq: public MsgPacket
{
public:
	ActiveReq(uint32_t seq = 0)
	{
		_header._type = ACTIVE_REQ;
		_header._seq = seq;
		_load = 0;
	}

	virtual ~ActiveReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int32(_load);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int32(_load);
		return true;
	}

public:
	uint32_t _load;
};

class ActiveRsp: public MsgPacket
{
public:
	ActiveRsp(uint32_t seq = 0)
	{
		_header._type = ACTIVE_RSP;
		_header._seq = seq;
		_load = 0;
	}

	virtual ~ActiveRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int32(_load);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int32(_load);
		return true;
	}

public:
	uint32_t _load;
};

class SrvListReq: public MsgPacket
{
public:
	SrvListReq(uint32_t seq = 0)
	{
		_header._type = SRVLIST_REQ;
		_header._seq = seq;
		_type = LOGIN_TYPE_WAS;
	}

	virtual ~SrvListReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int32(_type);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int32(_type);
		return true;
	}

public:
	uint32_t _type;
};

class SrvListRsp: public MsgPacket
{
public:
	struct Srv
	{
		uint32_t _load;  // 服务的负载
		uint16_t _port;  // 服务的端口
		std::string _ip;    // 服务的 IP，NOTE: 长度最大为 255 字节。
	};

	typedef std::list<Srv> SrvList;

public:
	SrvListRsp(uint32_t seq = 0)
	{
		_header._type = SRVLIST_RSP;
		_header._seq = seq;
		_result = 1;
		_type = 0;
		_count = 0;
	}

	virtual ~SrvListRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_int32(_type);
		_count = _srv_list.size();
		pack->write_int16(_count);

		SrvList::iterator it = _srv_list.begin();
		for (uint16_t i = 0; i < _count && it != _srv_list.end(); ++i, ++it)
		{
			Srv& srv = *it;

			pack->write_int32(srv._load);
			pack->write_int16(srv._port);
			pack->write_str(srv._ip);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_int32(_type);
		pack->read_int16(_count);

		for (uint16_t i = 0; i < _count; ++i)
		{
			Srv srv;

			pack->read_int32(srv._load);
			pack->read_int16(srv._port);
			pack->read_str(srv._ip);

			_srv_list.push_back(srv);
		}
		return true;
	}

public:
	uint8_t _result;  // 结果
	uint32_t _type;
	uint16_t _count;   //
	SrvList _srv_list;
};

class SrvListMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	SrvListMod(uint32_t seq = 0)
	{
		_header._type = SRVLIST_MOD;
		_header._seq = seq;
		_type = Add;
		_srv_type = 0;
		_srv_port = 0;
	}

	virtual ~SrvListMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_int32(_srv_type);
		pack->write_int16(_srv_port);
		pack->write_str(_srv_ip);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_int32(_srv_type);
		pack->read_int16(_srv_port);
		pack->read_str(_srv_ip);

		return true;
	}

public:
	uint8_t _type;      // 0:添加/更新， 1：删除
	uint32_t _srv_type;  // 服务的登录类型
	uint16_t _srv_port;  // 服务的端口
	std::string _srv_ip;    // 服务的 IP，NOTE: 长度最大为 255 字节。
};

class SrvListModRsp: public MsgPacket
{
public:
	SrvListModRsp(uint32_t seq = 0)
	{
		_header._type = SRVLIST_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~SrvListModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;  // 结果
};

// 基本信息
class VehicleInfoReq: public MsgPacket
{
public:
	VehicleInfoReq(uint32_t seq = 0)
	{
		_header._type = VEHICLE_INFO_REQ;
		_header._seq = seq;
	}

	virtual ~VehicleInfoReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		return true;
	}

public:
	std::string _phone;    // 手机号，NOTE: 长度最大为 255 字节。
};

class VehicleInfoRsp: public MsgPacket
{
public:
	VehicleInfoRsp(uint32_t seq = 0)
	{
		_header._type = VEHICLE_INFO_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~VehicleInfoRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_phone);
		pack->write_str(_flag);
		pack->write_str(_color);
		pack->write_str(_vehicle);
		pack->write_str(_term);
		pack->write_str(_vin);
		pack->write_str(_vid);
		pack->write_str(_auth);
		pack->write_str(_corp);
		pack->write_str(_city);
		pack->write_str(_oem);
		pack->write_str(_term_type);
		pack->write_str(_platform);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_phone);
		pack->read_str(_flag);
		pack->read_str(_color);
		pack->read_str(_vehicle);
		pack->read_str(_term);
		pack->read_str(_vin);
		pack->read_str(_vid);
		pack->read_str(_auth);
		pack->read_str(_corp);
		pack->read_str(_city);
		pack->read_str(_oem);
		pack->read_str(_term_type);
		pack->read_str(_platform);

		return true;
	}

public:
	uint8_t _result;     // 结果
	std::string _phone;      // 手机号，NOTE: 长度最大为 255 字节。
	std::string _flag;       // 是否有效标志位，NOTE: 长度应该为 1 字节。
	std::string _color;      // 车牌颜色，NOTE: 长度最大为 255 字节。
	std::string _vehicle;    // 车牌号，NOTE: 长度最大为 255 字节。
	std::string _term;       // 终端ID，NOTE: 长度最大为 255 字节。
	std::string _vin;        // NOTE: 长度最大为 255 字节。
	std::string _vid;        // NOTE: 长度最大为 255 字节。
	std::string _auth;       // 鉴权码，NOTE: 长度最大为 255 字节。
	std::string _corp;       // 企业ID，NOTE: 长度最大为 255 字节。
	std::string _city;       // 城市ID，NOTE: 长度最大为 255 字节。
	std::string _oem;        // 制造商ID，NOTE: 长度最大为 255 字节。
	std::string _term_type;  // 终端类型，ZD-V02H，NOTE: 长度最大为 255 字节。
	std::string _platform;   // 平台 ID
};

class VehicleInfoMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	VehicleInfoMod(uint32_t seq = 0)
	{
		_header._type = VEHICLE_INFO_MOD;
		_header._seq = seq;
		_type = Add;
	}

	virtual ~VehicleInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);

		if (_type == Add)
		{
			pack->write_str(_flag);
			pack->write_str(_color);
			pack->write_str(_vehicle);
			pack->write_str(_term);
			pack->write_str(_vin);
			pack->write_str(_vid);
			pack->write_str(_auth);
			pack->write_str(_corp);
			pack->write_str(_city);
			pack->write_str(_oem);
			pack->write_str(_term_type);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);

		if (_type == Add)
		{
			pack->read_str(_flag);
			pack->read_str(_color);
			pack->read_str(_vehicle);
			pack->read_str(_term);
			pack->read_str(_vin);
			pack->read_str(_vid);
			pack->read_str(_auth);
			pack->read_str(_corp);
			pack->read_str(_city);
			pack->read_str(_oem);
			pack->read_str(_term_type);
		}
		return true;
	}

public:
	uint8_t _type;       // 0:添加/更新， 1：删除
	std::string _phone;      // 手机号，NOTE: 长度最大为 255 字节。
	std::string _flag;       // 是否有效标志位，NOTE: 长度应该为 1 字节。
	std::string _color;      // 车牌颜色，NOTE: 长度最大为 255 字节。
	std::string _vehicle;    // 车牌号，NOTE: 长度最大为 255 字节。
	std::string _term;       // 终端ID，NOTE: 长度最大为 255 字节。
	std::string _vin;        // NOTE: 长度最大为 255 字节。
	std::string _vid;        // NOTE: 长度最大为 255 字节。
	std::string _auth;       // 鉴权码，NOTE: 长度最大为 255 字节。
	std::string _corp;       // 企业ID，NOTE: 长度最大为 255 字节。
	std::string _city;       // 城市ID，NOTE: 长度最大为 255 字节。
	std::string _oem;        // 制造商ID，NOTE: 长度最大为 255 字节。
	std::string _term_type;  // 终端类型，ZD-V02H，NOTE: 长度最大为 255 字节。
};

class VehicleInfoModRsp: public MsgPacket
{
public:
	VehicleInfoModRsp(uint32_t seq = 0)
	{
		_header._type = VEHICLE_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~VehicleInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;     // 结果
};

class OemTermInfoReq: public MsgPacket
{
public:
	OemTermInfoReq(uint32_t seq = 0)
	{
		_header._type = OEM_TERM_INFO_REQ;
		_header._seq = seq;
	}

	virtual ~OemTermInfoReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oem);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oem);
		return true;
	}

public:
	std::string _oem;    // NOTE: 长度最大为 255 字节。
};

class OemTermInfoRsp: public MsgPacket
{
public:
	OemTermInfoRsp(uint32_t seq = 0)
	{
		_header._type = OEM_TERM_INFO_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~OemTermInfoRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_oem);
		pack->write_str(_term_types);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_oem);
		pack->read_str(_term_types);

		return true;
	}

public:
	uint8_t _result;      // 结果
	std::string _oem;         // NOTE: 长度最大为 255 字节。
	std::string _term_types;  // 终端类型，NOTE: 长度最大为 255 字节。
};

class OemTermInfoMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	OemTermInfoMod(uint32_t seq = 0)
	{
		_header._type = OEM_TERM_INFO_MOD;
		_header._seq = seq;
		_type = Add;
	}

	virtual ~OemTermInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_oem);

		if (_type == Add)
		{
			pack->write_str(_term_types);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_oem);

		if (_type == Add)
		{
			pack->read_str(_term_types);
		}

		return true;
	}

public:
	uint8_t _type;  // 0:添加/更新， 1：删除
	std::string _oem;   // NOTE: 长度最大为 255 字节。
	std::string _term_types;  // 终端类型，NOTE: 长度最大为 255 字节。
};

class OemTermInfoModRsp: public MsgPacket
{
public:
	OemTermInfoModRsp(uint32_t seq = 0)
	{
		_header._type = OEM_TERM_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~OemTermInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;     // 结果
};

class OemRegInfoReq: public MsgPacket
{
public:
	OemRegInfoReq(uint32_t seq = 0)
	{
		_header._type = OEM_REG_INFO_REQ;
		_header._seq = seq;
	}
	virtual ~OemRegInfoReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oem);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oem);
		return true;
	}

public:
	std::string _oem;    // NOTE: 长度最大为 255 字节。
};

class OemRegInfoRsp: public MsgPacket
{
public:
	OemRegInfoRsp(uint32_t seq = 0)
	{
		_header._type = OEM_REG_INFO_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~OemRegInfoRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_oem);
		pack->write_str(_reg);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_oem);
		pack->read_str(_reg);

		return true;
	}
public:
	uint8_t _result;  // 结果
	std::string _oem; // NOTE: 长度最大为 255 字节。
	std::string _reg; // 注册码，KEY1_KEY2_KEY3，NOTE: 长度最大为 255 字节。
};

class OemRegInfoMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	OemRegInfoMod(uint32_t seq = 0)
	{
		_header._type = OEM_REG_INFO_MOD;
		_header._seq = seq;
		_type = Add;
	}

	virtual ~OemRegInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_oem);

		if (_type == Add)
		{
			pack->write_str(_reg);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_oem);

		if (_type == Add)
		{
			pack->read_str(_reg);
		}

		return true;
	}

public:
	uint8_t _type;  // 0:添加/更新， 1：删除
	std::string _oem;   // NOTE: 长度最大为 255 字节。
	std::string _reg;   // 注册码，KEY1_KEY2_KEY3，NOTE: 长度最大为 255 字节。
};

class OemRegInfoModRsp: public MsgPacket
{
public:
	OemRegInfoModRsp(uint32_t seq = 0)
	{
		_header._type = OEM_REG_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~OemRegInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;     // 结果
};

//////////////////////////////////////////////////////////////////////////////////
// MARK: 前置机相关
class WASRegCheck: public MsgPacket
{
public:
	WASRegCheck(uint32_t seq = 0)
	{
		_header._type = WAS_REG_CHECK;
		_header._seq = seq;
	}

	virtual ~WASRegCheck()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_color);
		pack->read_str(_vehicle);
		pack->read_str(_phone);
		pack->read_str(_term_type);
		pack->read_str(_term_id);
		pack->read_str(_oem);
		pack->read_str(_city);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_color);
		pack->write_str(_vehicle);
		pack->write_str(_phone);
		pack->write_str(_term_type);
		pack->write_str(_term_id);
		pack->write_str(_oem);
		pack->write_str(_city);
	}

public:
	std::string _color;  	 // 车牌颜色，NOTE: 长度最大为 255 字节。
	std::string _vehicle;	 // 车牌号，NOTE: 长度最大为 255 字节。
	std::string _phone;	 // 手机号，NOTE: 长度最大为 255 字节。
	std::string _term_type; // 终端类型，NOTE: 长度最大为 255 字节。
	std::string _term_id;	 // 终端ID，NOTE: 长度最大为 255 字节。
	std::string _oem;	     // 制造商ID，NOTE: 长度最大为 255 字节。
	std::string _city;		 // 城市ID，NOTE: 长度最大为 255 字节。
};

class WASRegCheckRsp: public MsgPacket
{
public:
	WASRegCheckRsp(uint32_t seq = 0)
	{
		_header._type = WAS_REG_CHECK_RSP;
		_header._seq = seq;
		_result = 1;
		_is_black = 0; // 默认不是黑名单
		_black_reason = 0;
		_black_time_sec = 0;
		_black_time_usec = 0;
	}

	virtual ~WASRegCheckRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_auth);
		pack->write_str(_oem);
		pack->write_str(_city);
		pack->write_int8(_is_black);
		pack->write_int8(_black_reason);
		pack->write_int64(_black_time_sec);
		pack->write_int64(_black_time_usec);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_auth);
		pack->read_str(_oem);
		pack->read_str(_city);
		pack->read_int8(_is_black);
		pack->read_int8(_black_reason);
		pack->read_int64(_black_time_sec);
		pack->read_int64(_black_time_usec);
		return true;
	}

public:
	uint8_t     _result;    // 注册结果
	std::string _auth;		// 鉴权码，NOTE: 长度最大为 255 字节。
	std::string _oem;		// 设备 OME，NOTE: 长度最大为 255 字节。
	std::string _city;      // 城市 ID，NOTE: 长度最大为 255 字节。
	uint8_t     _is_black;			// 是否为黑名单
	uint8_t     _black_reason;		// 如果是，当时加入黑名单原因
	uint64_t    _black_time_sec;	// 如果是，当时加入黑名单的时间
	uint64_t    _black_time_usec;	// 如果是，当时加入黑名单的时间
};

class WASAuthCheck: public MsgPacket
{
public:
	WASAuthCheck(uint32_t seq = 0)
	{
		_header._type = WAS_AUTH_CHECK;
		_header._seq = seq;
	}

	virtual ~WASAuthCheck()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_str(_auth);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_str(_auth);
		return true;
	}

public:
	std::string _phone;   // 手机号，NOTE: 长度最大为 255 字节。
	std::string _auth;	   // 鉴权码，NOTE: 长度最大为 255 字节。
};

class WASAuthCheckRsp: public MsgPacket
{
public:
	WASAuthCheckRsp(uint32_t seq = 0)
	{
		_header._type = WAS_AUTH_CHECK_RSP;
		_header._seq = seq;
		_result = 1;
		_is_black = 0; // 默认不是黑名单
		_black_reason = 0;
		_black_time_sec = 0;
		_black_time_usec = 0;
	}

	virtual ~WASAuthCheckRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_oem);
		pack->write_str(_city);
		pack->write_int8(_is_black);
		pack->write_int8(_black_reason);
		pack->write_int64(_black_time_sec);
		pack->write_int64(_black_time_usec);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_oem);
		pack->read_str(_city);
		pack->read_int8(_is_black);
		pack->read_int8(_black_reason);
		pack->read_int64(_black_time_sec);
		pack->read_int64(_black_time_usec);
		return true;
	}

public:
	uint8_t     _result;  // 结果
	std::string _oem;     // OEM码，NOTE: 长度最大为 255 字节。
	std::string _city;    // 城市 ID，NOTE: 长度最大为 255 字节。
	uint8_t     _is_black;			// 是否为黑名单
	uint8_t     _black_reason;		// 如果是，当时加入黑名单原因
	uint64_t    _black_time_sec;	// 如果是，当时加入黑名单的时间
	uint64_t    _black_time_usec;	// 如果是，当时加入黑名单的时间
};

class WasDriverCheck: public MsgPacket
{
public:
	WasDriverCheck(uint32_t seq = 0)
	{
		_header._type = WAS_DRIVER_CHECK;
		_header._seq = seq;
	}
	virtual ~WasDriverCheck()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_str(_driver_no);
		pack->write_str(_driver_id);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_str(_driver_no);
		pack->read_str(_driver_id);
		return true;
	}

public:
	std::string _phone;       // 手机号，NOTE: 长度最大为 255 字节。
	std::string _driver_no;    // 司机证号，NOTE: 长度最大为 255 字节。
	std::string _driver_id;    // 身份证号，NOTE: 长度最大为 255 字节。
};

class WasDriverCheckRsp: public MsgPacket
{
public:
	WasDriverCheckRsp(uint32_t seq = 0)
	{
		_header._type = WAS_DRIVER_CHECK_RSP;
		_header._seq = seq;
		_result = 1;
		_is_black = 0; // 默认不是黑名单
		_black_reason = 0;
		_black_time_sec = 0;
		_black_time_usec = 0;
	}

	virtual ~WasDriverCheckRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_msg);
		pack->write_int8(_is_black);
		pack->write_int8(_black_reason);
		pack->write_int64(_black_time_sec);
		pack->write_int64(_black_time_usec);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_msg);
		pack->read_int8(_is_black);
		pack->read_int8(_black_reason);
		pack->read_int64(_black_time_sec);
		pack->read_int64(_black_time_usec);
		return true;
	}

public:
	uint8_t _result;   // 识别结果
	std::string _msg;      // 身份数据信息，NOTE: 长度最大为 255 字节。
	uint8_t _is_black;			// 是否为黑名单
	uint8_t _black_reason;		// 如果是，当时加入黑名单原因
	uint64_t _black_time_sec;	// 如果是，当时加入黑名单的时间
	uint64_t _black_time_usec;	// 如果是，当时加入黑名单的时间
};

class WasLogout: public MsgPacket
{
public:
	WasLogout(uint32_t seq = 0)
	{
		_header._type = WAS_LOGOUT;
		_header._seq = seq;
	}
	virtual ~WasLogout()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
	}

	virtual bool unbody(Packet *pack)
	{
		if (pack->read_str(_phone) == 0)
			return false;
		return true;
	}

public:
	std::string _phone;   // 手机号，NOTE: 长度最大为 255 字节。
};

class WasLogoutRsp: public MsgPacket
{
public:
	WasLogoutRsp(uint32_t seq = 0)
	{
		_header._type = WAS_LOGOUT_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~WasLogoutRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}
	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;
};

class WasVehicleInfoMod: public MsgPacket
{
public:
	enum Mask
	{
		MaskNone = 0x00,
		MaskOEM = 0x01,
		MaskAuth = 0x02,
		MaskCity = 0x04,
		MaskFlag = 0x08,
		MaskAll = 0x7F,
		MaskDel = 0x80
	};

public:
	WasVehicleInfoMod(uint32_t seq = 0)
	{
		_header._type = WAS_VEHICLE_INFO_MOD;
		_header._seq = seq;
		_type = MaskNone;
	}
	virtual ~WasVehicleInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);
		if (_type & MaskOEM)
		{
			pack->write_str(_oem);
		}
		if (_type & MaskAuth)
		{
			pack->write_str(_auth);
		}
		if (_type & MaskCity)
		{
			pack->write_str(_city);
		}
		if (_type & MaskFlag)
		{
			pack->write_str(_flag);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);
		if (_type & MaskOEM)
		{
			pack->read_str(_oem);
		}
		if (_type & MaskAuth)
		{
			pack->read_str(_auth);
		}
		if (_type & MaskCity)
		{
			pack->read_str(_city);
		}
		if (_type & MaskFlag)
		{
			pack->read_str(_flag);
		}
		return true;
	}

public:
	uint8_t _type;  // 0:添加/更新， 1：删除
	std::string _phone; // 手机号，NOTE: 长度最大为 255 字节。
	std::string _oem;   // NOTE: 长度最大为 255 字节。
	std::string _auth;  // 鉴权码，NOTE: 长度最大为 255 字节。
	std::string _city;  // 省域 ID，NOTE: 长度最大为 255 字节。
	std::string _flag;  // 是否已经注册的标志，NOTE: 长度最大为 255 字节。
};

class WasVehicleInfoModRsp: public MsgPacket
{
public:
	WasVehicleInfoModRsp(uint32_t seq = 0)
	{
		_header._type = WAS_VEHICLE_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~WasVehicleInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}
public:
	uint8_t _result;     // 结果
};

class WasBlackMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	WasBlackMod(uint32_t seq = 0)
	{
		_header._type = WAS_BLACK_MOD;
		_header._seq = seq;
		_type = Add;
		_black_time_sec = 0;
		_black_time_usec = 0;
		_black_reason = 0;
	}

	virtual ~WasBlackMod()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);
		pack->read_time(_black_time_sec);
		pack->read_time(_black_time_usec);
		pack->read_int8(_black_reason);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);
		pack->write_time(_black_time_sec);
		pack->write_time(_black_time_usec);
		pack->write_int8(_black_reason);
	}

public:
	uint8_t _type;  			// 0:添加/更新， 1：删除
	std::string _phone; 			// 手机号，NOTE: 长度最大为 255 字节。
	uint64_t _black_time_sec;	// 加黑时间,秒部分
	uint64_t _black_time_usec;	// 加黑时间，微秒部分
	uint8_t _black_reason;		// 加黑原因
};

class WasBlackModRsp: public MsgPacket
{
public:
	WasBlackModRsp(uint32_t seq = 0)
	{
		_header._type = WAS_BLACK_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~WasBlackModRsp()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

public:
	uint8_t _result;  // 0 成功，1失败
};

//////////////////////////////////////////////////////////////////////////////////
// MARK: 透传相关
class DPTermAuthReq: public MsgPacket
{
public:
	DPTermAuthReq(uint32_t seq = 0)
	{
		_header._type = DP_TERM_AUTH_REQ;
		_header._seq = seq;
	}
	virtual ~DPTermAuthReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_str(_corp);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_str(_corp);
		return true;
	}

public:
	std::string _phone;  // NOTE: 长度最大为 255 字节。
	std::string _corp;   // NOTE: 长度最大为 255 字节。

};

class DPTermAuthRsp: public MsgPacket
{
public:
	DPTermAuthRsp(uint32_t seq = 0)
	{
		_header._type = DP_TERM_AUTH_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~DPTermAuthRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_phone);
		pack->write_str(_corp);
		pack->write_str(_auth);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_phone);
		pack->read_str(_corp);
		pack->read_str(_auth);

		return true;
	}
public:
	uint8_t _result;      // 结果
	std::string _phone;  // NOTE: 长度最大为 255 字节。
	std::string _corp;   // NOTE: 长度最大为 255 字节。
	std::string _auth;   // NOTE: 长度最大为 255 字节。
};

class DPTermAuthMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01, TryDel = 0x02 // 当前值与预删除值一致才删除，否则不执行删除动作。
	};

public:
	DPTermAuthMod(uint32_t seq = 0)
	{
		_header._type = DP_TERM_AUTH_MOD;
		_header._seq = seq;
		_type = Add;
	}

	virtual ~DPTermAuthMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);
		pack->write_str(_corp);

		// 删除前需确定已存在的和要删除的是同一个鉴权码
		// 所以需要删除操作带着预删除的鉴权码
		//if (_type == Add) {
		pack->write_str(_auth);
		//}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);
		pack->read_str(_corp);

		//if (_type == Add) {
		pack->read_str(_auth);
		//}

		return true;
	}

public:
	uint8_t _type;   // 0:添加/更新， 1：删除
	std::string _phone;  // NOTE: 长度最大为 255 字节。
	std::string _corp;   // NOTE: 长度最大为 255 字节。
	std::string _auth;   // NOTE: 长度最大为 255 字节。
};

class DPTermAuthModRsp: public MsgPacket
{
public:
	DPTermAuthModRsp(uint32_t seq = 0)
	{
		_header._type = DP_TERM_AUTH_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~DPTermAuthModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}
public:
	uint8_t _result;     // 结果
};

class DPCorpInfoReq: public MsgPacket
{
public:
	DPCorpInfoReq(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_REQ;
		_header._seq = seq;
	}
	virtual ~DPCorpInfoReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_corp);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_corp);
		return true;
	}

public:
	std::string _corp;   // NOTE: 长度最大为 255 字节。
};

class DPCorpInfoRsp: public MsgPacket
{
public:
	DPCorpInfoRsp(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_RSP;
		_header._seq = seq;
		_result = 1;
		_master_tcp_port = 0;
		_master_udp_port = 0;
		_slave_tcp_port = 0;
		_slave_udp_port = 0;
	}

	virtual ~DPCorpInfoRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_corp_id);
		pack->write_str(_master_ip);
		pack->write_int16(_master_tcp_port);
		pack->write_int16(_master_udp_port);
		pack->write_str(_slave_ip);
		pack->write_int16(_slave_tcp_port);
		pack->write_int16(_slave_udp_port);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_corp_id);
		pack->read_str(_master_ip);
		pack->read_int16(_master_tcp_port);
		pack->read_int16(_master_udp_port);
		pack->read_str(_slave_ip);
		pack->read_int16(_slave_tcp_port);
		pack->read_int16(_slave_udp_port);

		return true;
	}

public:
	uint8_t     _result;            // 结果
	std::string _corp_id;           // 企业ID，NOTE: 长度最大为 255 字节。
	std::string _master_ip;         // 主 IP
	uint16_t    _master_tcp_port;   // 主 TCP 端口
	uint16_t    _master_udp_port;   // 主 UDP 端口
	std::string _slave_ip;          // 从 IP
	uint16_t    _slave_tcp_port;    // 从 TCP 端口
	uint16_t    _slave_udp_port;    // 从 UPD 端口
};

class DPCorpInfoMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

public:
	DPCorpInfoMod(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_MOD;
		_header._seq = seq;
		_type = Add;
		_master_tcp_port = 0;
		_master_udp_port = 0;
		_slave_tcp_port = 0;
		_slave_udp_port = 0;
	}
	virtual ~DPCorpInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_corp_id);

		if (_type == Add)
		{
			pack->write_str(_master_ip);
			pack->write_int16(_master_tcp_port);
			pack->write_int16(_master_udp_port);
			pack->write_str(_slave_ip);
			pack->write_int16(_slave_tcp_port);
			pack->write_int16(_slave_udp_port);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_corp_id);

		if (_type == Add)
		{
			pack->read_str(_master_ip);
			pack->read_int16(_master_tcp_port);
			pack->read_int16(_master_udp_port);
			pack->read_str(_slave_ip);
			pack->read_int16(_slave_tcp_port);
			pack->read_int16(_slave_udp_port);
		}

		return true;
	}

public:
	uint8_t     _type;              // 0:添加/更新， 1：删除
	std::string _corp_id;           // 企业ID，NOTE: 长度最大为 255 字节。
	std::string _master_ip;         // 主 IP
	uint16_t    _master_tcp_port;   // 主 TCP 端口
	uint16_t    _master_udp_port;   // 主 UDP 端口
	std::string _slave_ip;          // 从 IP
	uint16_t    _slave_tcp_port;    // 从 TCP 端口
	uint16_t    _slave_udp_port;    // 从 UPD 端口
};

class DPCorpInfoModRsp: public MsgPacket
{
public:
	DPCorpInfoModRsp(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~DPCorpInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;  // 结果
};

class DPCorpInfoAllReq: public MsgPacket
{
public:
	DPCorpInfoAllReq(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_ALL_REQ;
		_header._seq = seq;
	}

	virtual ~DPCorpInfoAllReq()
	{
	}

	virtual void body(Packet */*pack*/)
	{
		// 消息体为空
	}

	virtual bool unbody(Packet */*pack*/)
	{
		// 消息体为空
		return true;
	}

public:

};

class DPCorpInfoAllRsp: public MsgPacket
{
public:
	struct CorpInfo
	{
		std::string _corp_id;        // 企业ID，NOTE: 长度最大为 255 字节。
		std::string _master_ip;      // 主 IP
		uint16_t _master_tcp_port;   // 主 TCP 端口
		uint16_t _master_udp_port;   // 主 UDP 端口
		std::string _slave_ip;       // 从 IP
		uint16_t _slave_tcp_port;    // 从 TCP 端口
		uint16_t _slave_udp_port;    // 从 UPD 端口
	};
	typedef std::list<CorpInfo> CorpInfoList;

public:
	DPCorpInfoAllRsp(uint32_t seq = 0)
	{
		_header._type = DP_CORP_INFO_ALL_RSP;
		_header._seq = seq;
		_result = 1;
		_corp_count = 0;
	}
	virtual ~DPCorpInfoAllRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);

		_corp_count = _corp_list.size();
		pack->write_int16(_corp_count);

		CorpInfoList::iterator it = _corp_list.begin();
		for (uint16_t i = 0; i < _corp_count && it != _corp_list.end();
				++i, ++it)
		{
			CorpInfo& corp = *it;

			pack->write_str(corp._corp_id);
			pack->write_str(corp._master_ip);
			pack->write_int16(corp._master_tcp_port);
			pack->write_int16(corp._master_udp_port);
			pack->write_str(corp._slave_ip);
			pack->write_int16(corp._slave_tcp_port);
			pack->write_int16(corp._slave_udp_port);
		}

	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_int16(_corp_count);

		for (uint16_t i = 0; i < _corp_count; ++i)
		{
			CorpInfo corp;

			pack->read_str(corp._corp_id);
			pack->read_str(corp._master_ip);
			pack->read_int16(corp._master_tcp_port);
			pack->read_int16(corp._master_udp_port);
			pack->read_str(corp._slave_ip);
			pack->read_int16(corp._slave_tcp_port);
			pack->read_int16(corp._slave_udp_port);

			_corp_list.push_back(corp);
		}

		return true;
	}
public:
	uint8_t _result;       // 结果
	uint16_t _corp_count;   // 企业信息列表长度
	CorpInfoList _corp_list;    // 企业信息列表
};

class DPTermCorpReq: public MsgPacket
{
public:
	DPTermCorpReq(uint32_t seq = 0)
	{
		_header._type = DP_TERM_CORP_REQ;
		_header._seq = seq;
	}
	virtual ~DPTermCorpReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		return true;
	}

public:
	std::string _phone;  // NOTE: 长度最大为 255 字节。
};

class DPTermCorpRsp: public MsgPacket
{
public:
	typedef std::set<std::string> CorpSet;

public:
	DPTermCorpRsp(uint32_t seq = 0)
	{
		_header._type = DP_TERM_CORP_RSP;
		_header._seq = seq;
		_result = 1;
		_corp_count = 0;
	}
	virtual ~DPTermCorpRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_phone);
		_corp_count = _corp_set.size();
		pack->write_int16(_corp_count);

		CorpSet::iterator it = _corp_set.begin();
		for (uint16_t i = 0; i < _corp_count && it != _corp_set.end();
				++i, ++it)
		{
			std::string corp = (*it).c_str();
			pack->write_str(corp);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_phone);
		pack->read_int16(_corp_count);

		for (uint16_t i = 0; i < _corp_count; ++i)
		{
			std::string corp;
			pack->read_str(corp);

			if (corp.empty())
			{
				continue;
			}

			_corp_set.insert(corp);
		}

		return true;
	}
public:
	uint8_t _result;      // 结果
	std::string _phone;       // NOTE: 长度最大为 255 字节。
	uint16_t _corp_count;  // 企业列表长度
	CorpSet _corp_set;    // 企业列表
};

class DPTermCorpMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Rem = 0x01, Set = 0x02, Del = 0x03
	};

	typedef std::set<std::string> CorpSet;

public:
	DPTermCorpMod(uint32_t seq = 0)
	{
		_header._type = DP_TERM_CORP_MOD;
		_header._seq = seq;
		_type = Add;
		_corp_count = 0;
	}
	virtual ~DPTermCorpMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);

		if (_type == Del)
		{
			return;
		}

		_corp_count = _corp_set.size();
		pack->write_int16(_corp_count);

		CorpSet::iterator it = _corp_set.begin();
		for (uint16_t i = 0; i < _corp_count && it != _corp_set.end();
				++i, ++it)
		{
			std::string corp = (*it).c_str();
			pack->write_str(corp);
		}

	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);

		if (_type == Del)
		{
			return true;
		}

		pack->read_int16(_corp_count);

		for (uint16_t i = 0; i < _corp_count; ++i)
		{
			std::string corp;
			pack->read_str(corp);

			if (corp.empty())
			{
				continue;
			}

			_corp_set.insert(corp);
		}
		return true;
	}

public:
	uint8_t _type;         // 0:添加/更新， 1：删除
	std::string _phone;        // NOTE: 长度最大为 255 字节。
	uint16_t _corp_count;   // 企业列表长度
	CorpSet _corp_set;    // 企业列表
};

class DPTermCorpModRsp: public MsgPacket
{
public:
	DPTermCorpModRsp(uint32_t seq = 0)
	{
		_header._type = DP_TERM_CORP_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~DPTermCorpModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;     // 结果
};

class DPSubsReq: public MsgPacket
{
public:

	enum OpType
	{
		Add = 0x00, Del = 0x01
	};

	typedef std::set<std::string> PhoneList;

public:
	DPSubsReq(uint32_t seq = 0)
	{
		_header._type = DP_SUBS_REQ;
		_header._seq = seq;
		_type = Add;
		_count = 0;
	}

	virtual ~DPSubsReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);

		_count = _phone_list.size();
		pack->write_int32(_count);

		PhoneList::iterator it = _phone_list.begin();
		for (uint32_t i = 0; i < _count && it != _phone_list.end(); ++i, ++it)
		{
			std::string phone = *it;
			pack->write_str(phone);
		}
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_int32(_count);

		for (uint32_t i = 0; i < _count; ++i)
		{
			std::string phone;
			pack->read_str(phone);

			_phone_list.insert(phone);
		}
		return true;
	}

public:
	uint8_t _type;       // 0:添加/更新， 1：删除
	uint32_t _count;      //
	PhoneList _phone_list; // 手机号列表
};

class DPSubsRsp: public MsgPacket
{
public:
	DPSubsRsp(uint32_t seq = 0)
	{
		_header._type = DP_SUBS_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~DPSubsRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}
public:
	uint8_t _result;  // 结果
};

class DPTransferReq: public MsgPacket
{
public:

	enum TransType
	{
		Tcp = 0x00, Udp = 0x01
	};

public:
	DPTransferReq(uint32_t seq = 0)
	{
		_header._type = DP_TRANSFER_REQ;
		_header._seq = seq;
		_type = Tcp;
	}
	virtual ~DPTransferReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);
		pack->write_str(_phone);
		pack->write_string(_data);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);
		pack->read_str(_phone);
		pack->read_string(_data);
		return true;
	}

public:
	uint8_t _type;    // 0:TCP， 1：UDP
	std::string _phone;   // 手机号，NOTE: 长度最大为 255 字节。
	std::string _data;    // 数据
};

class DPTransferRsp: public MsgPacket
{
public:
	DPTransferRsp(uint32_t seq = 0)
	{
		_header._type = DP_TRANSFER_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~DPTransferRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}
	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}
public:
	uint8_t _result;  // 结果
};

// PCC 相关
class PccVehicleInfoReq: public MsgPacket
{
public:
	PccVehicleInfoReq(uint32_t seq = 0)
	{
		_header._type = PCC_VEHICLE_INFO_REQ;
		_header._seq = seq;
	}
	virtual ~PccVehicleInfoReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_color_vehicle);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_color_vehicle);
		return true;
	}

public:
	std::string _color_vehicle;      // 车牌颜色_车牌号，NOTE: 长度最大为 255 字节。
};

class PccVehicleInfoRsp: public MsgPacket
{
public:
	PccVehicleInfoRsp(uint32_t seq = 0)
	{
		_header._type = PCC_VEHICLE_INFO_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~PccVehicleInfoRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
		pack->write_str(_color_vehicle);
		pack->write_str(_oem_phone);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		pack->read_str(_color_vehicle);
		pack->read_str(_oem_phone);

		return true;
	}
public:
	uint8_t _result;        // 结果
	std::string _color_vehicle; // 车牌颜色_车牌号，NOTE: 长度最大为 255 字节。
	std::string _oem_phone;     // 手机号，NOTE: 长度最大为 255 字节。
};

class PccVehicleInfoMod: public MsgPacket
{
public:
	enum OpType
	{
		Add = 0x00, Del = 0x01, DelAll = 0x02
	};

public:
	PccVehicleInfoMod(uint32_t seq = 0)
	{
		_header._type = PCC_VEHICLE_INFO_MOD;
		_header._seq = seq;
		_type = Add;
	}
	virtual ~PccVehicleInfoMod()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_type);

		pack->write_str(_oem);
		pack->write_str(_phone);

		pack->write_str(_color);
		pack->write_str(_vehicle);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_type);

		pack->read_str(_oem);
		pack->read_str(_phone);

		pack->read_str(_color);
		pack->read_str(_vehicle);
		return true;
	}

public:
	uint8_t _type;    // 0:添加/更新， 1：删除
	std::string _oem;
	std::string _phone;   // 手机号，NOTE: 长度最大为 255 字节。
	std::string _color;
	std::string _vehicle; // 车牌颜色_车牌号，NOTE: 长度最大为 255 字节。
};

class PccVehicleInfoModRsp: public MsgPacket
{
public:
	PccVehicleInfoModRsp(uint32_t seq = 0)
	{
		_header._type = PCC_VEHICLE_INFO_MOD_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~PccVehicleInfoModRsp()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_int8(_result);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_int8(_result);
		return true;
	}

public:
	uint8_t _result;     // 结果
};

//was ptrans 相关 xfm
class PtrnasForwardConnectReq: public MsgPacket
{
public:
	PtrnasForwardConnectReq(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_CONNECT_REQ;
		_header._seq = seq;
	}

	virtual ~PtrnasForwardConnectReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oemcode);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oemcode);
		return true;
	}

public:
	std::string _oemcode;
};

class PtrnasForwardConnectRsp: public MsgPacket
{
public:
	PtrnasForwardConnectRsp(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_CONNECT_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~PtrnasForwardConnectRsp()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oemcode);
		pack->read_int8(_result);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oemcode);
		pack->write_int8(_result);
	}

public:
	std::string _oemcode;
	uint8_t     _result;  // 0 成功，1失败
};

class PtrnasForwardDisconnectReq: public MsgPacket
{
public:
	PtrnasForwardDisconnectReq(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_DISCONNECT_REQ;
		_header._seq = seq;
	}

	virtual ~PtrnasForwardDisconnectReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oemcode);

	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oemcode);
		return true;
	}

public:
	std::string _oemcode;
};

class PtrnasForwardDisconnectRsp: public MsgPacket
{
public:
	PtrnasForwardDisconnectRsp(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_DISCONNECT_RSP;
		_header._seq = seq;
		_result = 1;
	}

	virtual ~PtrnasForwardDisconnectRsp()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_oemcode);
		pack->read_int8(_result);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_oemcode);
		pack->write_int8(_result);
	}

public:
	std::string _oemcode;
	uint8_t     _result;  // 0 成功，1失败
};

class PtrnasForwardUpReq: public MsgPacket
{
public:
	PtrnasForwardUpReq(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_UP_REQ;
		_header._seq = seq;
	}

	virtual ~PtrnasForwardUpReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_str(_authcode);
		pack->write_str(_oemcode);
		pack->write_str(_cityid);
		pack->write_string(_data);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_str(_authcode);
		pack->read_str(_oemcode);
		pack->read_str(_cityid);
		pack->read_string(_data);
		return true;
	}

public:
	std::string _phone;
	std::string _authcode;
	std::string _oemcode;
	std::string _cityid;
	std::string _data;
};

class PtrnasForwardUpRsp: public MsgPacket
{
public:
	PtrnasForwardUpRsp(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_UP_RSP;
		_header._seq = seq;
		_result = 1;
	}
	virtual ~PtrnasForwardUpRsp()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_int8(_result);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_int8(_result);
	}
public:
	std::string _phone;
	uint8_t     _result;  // 0 成功，1失败
};

class PtrnasForwardDownReq: public MsgPacket
{
public:
	PtrnasForwardDownReq(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_DOWN_REQ;
		_header._seq = seq;
	}

	virtual ~PtrnasForwardDownReq()
	{
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_string(_data);
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_string(_data);
		return true;
	}
public:
	std::string _phone;
	std::string _data;
};

class PtrnasForwardDownRsp: public MsgPacket
{
public:
	PtrnasForwardDownRsp(uint32_t seq = 0)
	{
		_header._type = PTRANS_FORWARD_DOWN_RSP;
		_header._seq  = seq;
		_result       = 1;
	}
	virtual ~PtrnasForwardDownRsp()
	{
	}

	virtual bool unbody(Packet *pack)
	{
		pack->read_str(_phone);
		pack->read_int8(_result);
		return true;
	}

	virtual void body(Packet *pack)
	{
		pack->write_str(_phone);
		pack->write_int8(_result);
	}

public:
	std::string _phone;
	uint8_t     _result;  // 0 成功，1失败
};

//////////////////////////////////////////////////////////////////////////////////
// MARK: 拆包器
class CCommProtoUnpackMgr: public IUnpackMgr
{
public:
	CCommProtoUnpackMgr()
	{
	}
	virtual ~CCommProtoUnpackMgr()
	{
	}

	// 实现数据解包接口方法
	MsgPacket * unbody(unsigned short msgtype, Packet &pack)
	{
		MsgPacket *msg = NULL;
		switch (msgtype)
		{
		// 共用
		case LOGIN_REQ:
			msg = unpack < LoginReq > (pack, "LOGIN_REQ");
			break;
		case LOGIN_RSP:
			msg = unpack < LoginRsp > (pack, "LOGIN_RSP");
			break;
		case ACTIVE_REQ:
			msg = unpack < ActiveReq > (pack, "ACTIVE_REQ");
			break;
		case ACTIVE_RSP:
			msg = unpack < ActiveRsp > (pack, "ACTIVE_RSP");
			break;
		case SRVLIST_REQ:
			msg = unpack < SrvListReq > (pack, "SRVLIST_REQ");
			break;
		case SRVLIST_RSP:
			msg = unpack < SrvListRsp > (pack, "SRVLIST_RSP");
			break;
		case SRVLIST_MOD:
			msg = unpack < SrvListMod > (pack, "SRVLIST_MOD");
			break;
		case SRVLIST_MOD_RSP:
			msg = unpack < SrvListModRsp > (pack, "SRVLIST_MOD_RSP");
			break;

			//MARK: 基本信息
		case VEHICLE_INFO_REQ:
			msg = unpack < VehicleInfoReq > (pack, "VEHICLE_INFO_REQ");
			break;
		case VEHICLE_INFO_RSP:
			msg = unpack < VehicleInfoRsp > (pack, "VEHICLE_INFO_RSP");
			break;
		case VEHICLE_INFO_MOD:
			msg = unpack < VehicleInfoMod > (pack, "VEHICLE_INFO_MOD");
			break;
		case VEHICLE_INFO_MOD_RSP:
			msg = unpack < VehicleInfoModRsp > (pack, "VEHICLE_INFO_MOD");
			break;
		case OEM_TERM_INFO_REQ:
			msg = unpack < OemTermInfoReq > (pack, "OEM_TERM_INFO_REQ");
			break;
		case OEM_TERM_INFO_RSP:
			msg = unpack < OemTermInfoRsp > (pack, "OEM_TERM_INFO_REQ");
			break;
		case OEM_TERM_INFO_MOD:
			msg = unpack < OemTermInfoMod > (pack, "OEM_TERM_INFO_REQ");
			break;
		case OEM_TERM_INFO_MOD_RSP:
			msg = unpack < OemTermInfoModRsp > (pack, "OEM_TERM_INFO_MOD_RSP");
			break;
		case OEM_REG_INFO_REQ:
			msg = unpack < OemRegInfoReq > (pack, "OEM_REG_INFO_REQ");
			break;
		case OEM_REG_INFO_RSP:
			msg = unpack < OemRegInfoRsp > (pack, "OEM_REG_INFO_REQ");
			break;
		case OEM_REG_INFO_MOD:
			msg = unpack < OemRegInfoMod > (pack, "OEM_REG_INFO_MOD");
			break;
		case OEM_REG_INFO_MOD_RSP:
			msg = unpack < OemRegInfoModRsp > (pack, "OEM_REG_INFO_MOD_RSP");
			break;

			// MARK: 前置机相关
		case WAS_REG_CHECK:
			msg = unpack < WASRegCheck > (pack, "WAS_REG_CHECK");
			break;
		case WAS_REG_CHECK_RSP:
			msg = unpack < WASRegCheckRsp > (pack, "WAS_REG_CHECK");
			break;
		case WAS_AUTH_CHECK:
			msg = unpack < WASAuthCheck > (pack, "WAS_AUTH_CHECK");
			break;
		case WAS_AUTH_CHECK_RSP:
			msg = unpack < WASAuthCheckRsp > (pack, "WAS_AUTH_CHECK_RSP");
			break;
		case WAS_DRIVER_CHECK:
			msg = unpack < WasDriverCheck > (pack, "WAS_DRIVER_CHECK");
			break;
		case WAS_DRIVER_CHECK_RSP:
			msg = unpack < WasDriverCheckRsp > (pack, "WAS_DRIVER_CHECK_RSP");
			break;
		case WAS_LOGOUT:
			msg = unpack < WasLogout > (pack, "WAS_LOGOUT");
			break;
		case WAS_LOGOUT_RSP:
			msg = unpack < WasLogoutRsp > (pack, "WAS_LOGOUT");
			break;
		case WAS_VEHICLE_INFO_MOD:
			msg = unpack < WasVehicleInfoMod > (pack, "WAS_VEHICLE_INFO_MOD");
			break;
		case WAS_VEHICLE_INFO_MOD_RSP:
			msg = unpack < WasVehicleInfoModRsp > (pack, "WAS_VEHICLE_INFO_MOD");
			break;
		case WAS_BLACK_MOD:
			msg = unpack < WasBlackMod > (pack, "WAS_BLACK_MOD");
			break;
		case WAS_BLACK_MOD_RSP:
			msg = unpack < WasBlackModRsp > (pack, "WAS_BLACK_MOD_RSP");
			break;

			// MARK: 透传相关
		case DP_SUBS_REQ:
			msg = unpack < DPSubsReq > (pack, "DP_SUBS_REQ");
			break;
		case DP_SUBS_RSP:
			msg = unpack < DPSubsRsp > (pack, "DP_SUBS_RSP");
			break;
		case DP_TRANSFER_REQ:
			msg = unpack < DPTransferReq > (pack, "DP_TRANSFER_REQ");
			break;
		case DP_TRANSFER_RSP:
			msg = unpack < DPTransferRsp > (pack, "DP_TRANSFER_RSP");
			break;
		case DP_TERM_AUTH_REQ:
			msg = unpack < DPTermAuthReq > (pack, "DP_TERM_AUTH_REQ");
			break;
		case DP_TERM_AUTH_RSP:
			msg = unpack < DPTermAuthRsp > (pack, "DP_TERM_AUTH_RSP");
			break;
		case DP_TERM_AUTH_MOD:
			msg = unpack < DPTermAuthMod > (pack, "DP_TERM_AUTH_MOD");
			break;
		case DP_TERM_AUTH_MOD_RSP:
			msg = unpack < DPTermAuthModRsp > (pack, "DP_TERM_AUTH_MOD");
			break;
		case DP_CORP_INFO_REQ:
			msg = unpack < DPCorpInfoReq > (pack, "DP_CORP_INFO_REQ");
			break;
		case DP_CORP_INFO_RSP:
			msg = unpack < DPCorpInfoRsp > (pack, "DP_CORP_INFO_RSP");
			break;
		case DP_CORP_INFO_MOD:
			msg = unpack < DPCorpInfoMod > (pack, "DP_CORP_INFO_MOD");
			break;
		case DP_CORP_INFO_MOD_RSP:
			msg = unpack < DPCorpInfoModRsp > (pack, "DP_CORP_INFO_MOD_RSP");
			break;
		case DP_CORP_INFO_ALL_REQ:
			msg = unpack < DPCorpInfoAllReq > (pack, "DP_CORP_INFO_ALL_REQ");
			break;
		case DP_CORP_INFO_ALL_RSP:
			msg = unpack < DPCorpInfoAllRsp > (pack, "DP_CORP_INFO_ALL_REQ");
			break;
		case DP_TERM_CORP_REQ:
			msg = unpack < DPTermCorpReq > (pack, "DP_TERM_CORP_REQ");
			break;
		case DP_TERM_CORP_RSP:
			msg = unpack < DPTermCorpRsp > (pack, "DP_TERM_CORP_RSP");
			break;
		case DP_TERM_CORP_MOD:
			msg = unpack < DPTermCorpMod > (pack, "DP_TERM_CORP_MOD");
			break;
		case DP_TERM_CORP_MOD_RSP:
			msg = unpack < DPTermCorpModRsp > (pack, "DP_TERM_CORP_MOD_RSP");
			break;

			// PCC 相关
		case PCC_VEHICLE_INFO_REQ:
			msg = unpack < PccVehicleInfoReq > (pack, "PCC_VEHICLE_INFO_REQ");
			break;
		case PCC_VEHICLE_INFO_RSP:
			msg = unpack < PccVehicleInfoRsp > (pack, "PCC_VEHICLE_INFO_RSP");
			break;
		case PCC_VEHICLE_INFO_MOD:
			msg = unpack < PccVehicleInfoMod > (pack, "PCC_VEHICLE_INFO_MOD");
			break;
		case PCC_VEHICLE_INFO_MOD_RSP:
			msg = unpack < PccVehicleInfoModRsp > (pack, "PCC_VEHICLE_INFO_MOD_RSP");
			break;
			//was ptrans		case PTRANS_FORWARD_UP_REQ:
			msg = unpack < PtrnasForwardUpReq > (pack, "PTRANS_FORWARD_UP_REQ");
			break;
		case PTRANS_FORWARD_UP_RSP:
			msg = unpack < PtrnasForwardUpRsp > (pack, "PTRANS_FORWARD_UP_RSP");
			break;
		case PTRANS_FORWARD_DOWN_REQ:
			msg = unpack < PtrnasForwardDownReq > (pack, "PTRANS_FORWARD_DOWN_REQ");
			break;
		case PTRANS_FORWARD_DOWN_RSP:
			msg = unpack < PtrnasForwardDownRsp > (pack, "PTRANS_FORWARD_DOWN_RSP");
			break;
		case PTRANS_FORWARD_DISCONNECT_REQ:
			msg = unpack < PtrnasForwardDisconnectReq > (pack, "PTRANS_FORWARD_DISCONNECT_REQ");
			break;
		case PTRANS_FORWARD_DISCONNECT_RSP:
			msg = unpack < PtrnasForwardDisconnectRsp > (pack, "PTRANS_FORWARD_DISCONNECT_RSP");
			break;

		case PTRANS_FORWARD_CONNECT_REQ:
			msg = unpack < PtrnasForwardConnectReq > (pack, "PTRANS_FORWARD_CONNECT_REQ");
			break;
		case PTRANS_FORWARD_CONNECT_RSP:
			msg = unpack < PtrnasForwardConnectRsp > (pack, "PTRANS_FORWARD_CONNECT_RSP");
			break;
			// 其他
		default:
			break;
		}
		return msg;
	}
};

} // namespace triones

#endif // #ifndef __TRIONES_COMMPROTO_H__
