/**
 * author: Triones
 * date  : 2014-09-03
 */

#ifndef __TRIONES_GBHANDLER_H__
#define __TRIONES_GBHANDLER_H__

#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <iostream>
#include "../comm/ref.h"
#include "../comm/databuffer.h"
#include "gbheader.h"

namespace triones
{

#define   GB_PACK_SIZE             500

//判断某位是不是为1，�@果是大于0，不是为0，不修改value�?
#define  IS_BIT(value,num)   (value &(0x1u<<num))

//某位赋�?�?，修改value�?
#define  S_BIT(value,num)    (value |=(0x1u<<num))

//某位赋�?�?，修改value�?
#define  C_BIT(value,num)    (value &=(~(0x1u<<num)) )

// 驾驶员身份信�?
struct DRIVER_INFO
{
	char drivername[256];
	char driverid[21];
	char driverorgid[41];
	char orgname[256];
};

struct DRIVER_INFO_NEW808
{
	unsigned char state;
	//BCD :YY MM DD hh mm ss
	unsigned char _timer[6];
	unsigned char result;
	char name[255 + 1];
	char certification[20 + 1];
	char agent[255 + 1];
	//BCD :YY YY MM DD
	unsigned char expire[4];
};

class GBHandler: public Ref
{
public:
	GBHandler(){}
	virtual ~GBHandler(){};

	//打印头的相关信息
	// void HeaderDecode(GBheader *header, int len, int flag);

	PlatFormCommonResp build_plat_form_common_resp(const GBheader*reqheaderptr, unsigned short downreq, unsigned char result);
	//转换为内部的 位置�?
	std::string convert_gps_info(GpsInfo *gps_info, const char *append_data, int append_data_len);

	//
	bool convert_get_para(char *buf, int buf_len, std::string &data);
	//
	bool get_driver_info(const char *buf, int len, DRIVER_INFO &info);
	//
	bool get_driver_info(const char *buf, int len, DRIVER_INFO_NEW808 &);
	//
	bool check_driver( unsigned char *ptr, int len ) ;

	//把驾驶员信息转换为内部协�?
	std::string convert_driver_info(char *buf, int buf_len, unsigned char result);

	// 转换发动机能效数�?
	std::string convert_engeer(EngneerData *p);
	// 转换驾驶行为事件
	std::string convert_event_gps(GpsInfo *gps);

	// 构建参数设制
	bool build_param_set(DataBuffer *pbuf, std::map<std::string, std::string> &map, unsigned char &pnum);

	// 构建扩展参数上报处理
	bool convert_report(const char *pbuf, int len, std::string &data);
	// 构建参数查询处理
	bool convert_get_param2(const char *pbuf, int len, std::string &data);
	// 解析下发参数设置
	bool build_set_param2(DataBuffer &buf, std::map<std::string, std::string> &mp, unsigned char &num);
	// 解析扩展参数设置2
	bool build_set_param_2ex(std::vector<DataBuffer*> &vbuf, std::map<std::string, std::string> &mp);
	//
	bool convert_report_new808(const char *buf, int len, char * datap);
	//
	bool convert_term_upgrade_result_notify(const char * buf, int len, char * datap);

public:
	unsigned char get_check_sum(const char *buf, int len);

	std::string get_bcd_time(unsigned char bcd[6]);
	//20110304
	std::string get_date();
	//050507
	std::string get_time();
	// 转换成十六进
	void print_hex(unsigned char *p, int n, char *buf);
};

} // namespace triones

#endif // #ifndef __TRIONES_GBHANDLER_H__
