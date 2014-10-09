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

//鍒ゆ柇鏌愪綅鏄笉鏄负1锛屽鏋滄槸澶т簬0锛屼笉鏄负0锛屼笉淇敼value鍊�
#define  IS_BIT(value,num)   (value &(0x1u<<num))

//鏌愪綅璧嬪�涓�锛屼慨鏀箆alue鍊�
#define  S_BIT(value,num)    (value |=(0x1u<<num))

//鏌愪綅璧嬪�涓�锛屼慨鏀箆alue鍊�
#define  C_BIT(value,num)    (value &=(~(0x1u<<num)) )

// 椹鹃┒鍛樿韩浠戒俊鎭�
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

	//鎵撳嵃澶寸殑鐩稿叧淇℃伅
	// void HeaderDecode(GBheader *header, int len, int flag);

	PlatFormCommonResp build_plat_form_common_resp(const GBheader*reqheaderptr, unsigned short downreq, unsigned char result);
	//杞崲涓哄唴閮ㄧ殑 浣嶇疆鍖�
	std::string convert_gps_info(GpsInfo *gps_info, const char *append_data, int append_data_len);

	//鎶婅缃弬鏁拌浆鎹负鍐呴儴鍗忚
	bool convert_get_para(char *buf, int buf_len, std::string &data);
	// 鑾峰彇椹鹃┒鍛樿韩浠戒俊鎭�
	bool get_driver_info(const char *buf, int len, DRIVER_INFO &info);
	// 鑾峰彇椹鹃┒鍛樿韩浠戒俊鎭�  涓存椂鏂规
	bool get_driver_info(const char *buf, int len, DRIVER_INFO_NEW808 &);
	// 妫�祴鏄柊鐗堥┚椹跺憳杩樻槸鑰佺増
	bool check_driver( unsigned char *ptr, int len ) ;

	//鎶婇┚椹跺憳淇℃伅杞崲涓哄唴閮ㄥ崗璁�
	std::string convert_driver_info(char *buf, int buf_len, unsigned char result);

	// 杞崲鍙戝姩鏈鸿兘鏁堟暟鎹�
	std::string convert_engeer(EngneerData *p);
	// 杞崲椹鹃┒琛屼负浜嬩欢
	std::string convert_event_gps(GpsInfo *gps);

	// 鏋勫缓鍙傛暟璁惧埗
	bool build_param_set(DataBuffer *pbuf, std::map<std::string, std::string> &map, unsigned char &pnum);

	// 鏋勫缓鎵╁睍鍙傛暟涓婃姤澶勭悊
	bool convert_report(const char *pbuf, int len, std::string &data);
	// 鏋勫缓鍙傛暟鏌ヨ澶勭悊
	bool convert_get_param2(const char *pbuf, int len, std::string &data);
	// 瑙ｆ瀽涓嬪彂鍙傛暟璁剧疆
	bool build_set_param2(DataBuffer &buf, std::map<std::string, std::string> &mp, unsigned char &num);
	// 瑙ｆ瀽鎵╁睍鍙傛暟璁剧疆2
	bool build_set_param_2ex(std::vector<DataBuffer*> &vbuf, std::map<std::string, std::string> &mp);
	//鏂�08涓紝缁堢灞炴�搴旂瓟涓婃姤
	bool convert_report_new808(const char *buf, int len, char * datap);
	//鏂�08涓紝缁堢鍗囩骇缁撴灉閫氱煡涓婃姤
	bool convert_term_upgrade_result_notify(const char * buf, int len, char * datap);

public:
	unsigned char get_check_sum(const char *buf, int len);

	std::string get_bcd_time(unsigned char bcd[6]);
	//20110304
	std::string get_date();
	//050507
	std::string get_time();
	// 杞崲鎴愬崄鍏繘鍒�
	void print_hex(unsigned char *p, int n, char *buf);
};

} // namespace triones

#endif // #ifndef __TRIONES_GBHANDLER_H__
