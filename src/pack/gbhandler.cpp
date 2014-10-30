/**
 * author: Triones
 * date  : 2014-09-03
 */

#include <arpa/inet.h>
#include "../utils/base64.h"
#include "../utils/tools.h"
#include "../utils/str.h"
#include "../comm/comlog.h"
#include "gbhandler.h"


using namespace std;

#define  MAX_DWORD_INVALID   0xFFFFFFFF
#define  MAX_WORD_INVALID    0xFFFF

namespace triones
{

static unsigned int get_dword(const char *buf)
{
	unsigned int dword = 0;
	memcpy(&dword, buf, 4);
	dword = ntohl(dword);
	return dword;
}

static unsigned short get_word(const char *buf)
{
	unsigned short word = 0;
	memcpy(&word, buf, 2);
	word = ntohs(word);
	return word;
}

static char * get_buffer(const char *buf, char *sz, int len)
{
	memcpy(sz, buf, len);
	sz[len] = 0;
	return sz;
}

// 将字符串时间转为BCD时间
static string bcd_2_utc(char bcd[6])
{
	string stime;
	if (bcd_to_str(bcd, 6, stime))
		return "";

	int nyear = 0, nmon = 0, nday = 0, nhour = 0, nmin = 0, nsec = 0;
	sscanf(stime.c_str(), "%02d%02d%02d%02d%02d%02d", &nyear, &nmon, &nday, &nhour, &nmin, &nsec);

	struct tm tm;
	tm.tm_year = nyear + 100;
	tm.tm_mon = nmon - 1;
	tm.tm_mday = nday;
	tm.tm_hour = nhour;
	tm.tm_min = nmin;
	tm.tm_sec = nsec;

	time_t ntime = mktime(&tm);

	char buf[128] = { 0 };
	sprintf(buf, "%llu", (long long) ntime);
	return buf;

}
///////////////////////////// GbHandler /////////////////////////////////

PlatFormCommonResp GBHandler::build_plat_form_common_resp(const GBheader*reqheaderptr, unsigned short downreq, unsigned char result)
{
	PlatFormCommonResp pcommonresp;
	memcpy(&(pcommonresp.header), reqheaderptr, sizeof(GBheader));

	unsigned short prop = 0x0005;
	unsigned short msgid = 0x8001;
	msgid = htons(msgid);
	prop = htons(prop);

	//初妾化�?用回复消息头
	memcpy(&(pcommonresp.header.msgtype), &prop, sizeof(unsigned short));
	memcpy(&(pcommonresp.header.msgid), &msgid, sizeof(unsigned short));
	pcommonresp.header.seq = htons(downreq);

	pcommonresp.resp.resp_msg_id = reqheaderptr->msgid;
	pcommonresp.resp.resp_seq = reqheaderptr->seq;
	pcommonresp.resp.result = result;
	pcommonresp.check_sum = get_check_sum((const char*) (&pcommonresp) + 1, sizeof(PlatFormCommonResp) - 3);
	pcommonresp.end._end = 0x7e;

	return pcommonresp;
}

string GBHandler::convert_engeer(EngneerData *p)
{
	string dest;
	if (p == NULL)
		return dest;
	dest += "4:" + get_bcd_time((unsigned char*)p->time) + ",";
	dest += "210:" + to_string(ntohs(p->speed)) + ",";
	dest += "503:" + to_string(p->torque) + ",";
	dest += "504:" + to_string(p->position);

	return dest;
}

// 转换驾驶行为事件
string GBHandler::convert_event_gps(GpsInfo *gps)
{
	string dest;
	if (gps == NULL)
		return dest;

	// [起妾位置纬度][起妾位置经度][起妾位置高度][起妾位置速度][起妾位置方向][起妾位置时间]
	unsigned int lon = 0;
	unsigned int lat = 0;
	lon = (unsigned int) ntohl(gps->longitude) * 6 / 10;
	lat = (unsigned int) ntohl(gps->latitude) * 6 / 10;

	dest += "[" + to_string(lat) + "]";
	dest += "[" + to_string(lon) + "]";
	dest += "[" + to_string(ntohs(gps->heigth)) + "]";
	dest += "[" + to_string(ntohs(gps->speed)) + "]";
	dest += "[" + to_string(ntohs(gps->direction)) + "]";
	dest += "[" + get_bcd_time((unsigned char*)gps->date_time) + "]";

	return dest;
}

// 构建MAP的KEY和VALUE值的关系
static void add_map_key(const string &key, const string &val, map<string, string> &mp)
{
	if (key.empty()) {
		return;
	}

	map<string, string>::iterator it = mp.find(key);
	if (it == mp.end()) {
		mp.insert(pair<string, string>(key, val));
	} else {
		it->second += "|";
		it->second += val;
	}
}

static const string build_map_command(map<string, string> &mp)
{
	string sdata = "";
	if (mp.empty())
		return sdata;

	map<string, string>::iterator it;
	for (it = mp.begin(); it != mp.end(); ++it) {
		if (!sdata.empty()) {
			sdata += ",";
		}
		sdata += it->first + ":" + it->second;
	}
	return sdata;
}

static string i_to_decstr(unsigned int intger, unsigned int max, bool bflag)
{
	char buf[128] = { 0 };

	if (max == intger || (bflag && intger == 0xff)) {
		sprintf(buf, "%d", -1);
	} else {
		sprintf(buf, "%u", intger);
	}
	return string(buf);
}

string GBHandler::convert_gps_info(GpsInfo*gps_info, const char *append_data, int append_data_len)
{
	string dest;
	if (gps_info == NULL)
		return dest;

	unsigned int lon = 0;
	unsigned int lat = 0;
	lon = (unsigned int) ntohl(gps_info->longitude) * 6 / 10;
	lat = (unsigned int) ntohl(gps_info->latitude) * 6 / 10;

	map<string, string> mp;

	add_map_key("1", to_string(lon), mp);
	add_map_key("2", to_string(lat), mp);
	add_map_key("3", to_string(ntohs(gps_info->speed)), mp);
	add_map_key("4", get_bcd_time((unsigned char*)gps_info->date_time), mp);

	//正北方向�?，顺时针方向，单位为2度�?
	add_map_key("5", to_string(ntohs(gps_info->direction)), mp);
	add_map_key("6", to_string(ntohs(gps_info->heigth)), mp);

	// 添加当前接收时间
	char sz[128] = {0};
	snprintf( sz, sizeof(sz)-1, "%lu", (long)time(NULL) ) ;
	add_map_key("999", sz , mp ) ;

	/*张鹤高增�?***********************************************************/
	//DWORD,位置基本信息状�?位，B0~B15,参�?JT/T808-2011,Page15，表17
	unsigned int status = 0;
	memcpy(&status, &(gps_info->state), sizeof(unsigned int));
	status = ntohl(status);
	// 处理状�?
	add_map_key("8", to_string(status), mp);

	//解析报警标志�?
	int ala = 0;
	memcpy(&ala, &(gps_info->alarm), sizeof(int));
	ala = ntohl(ala);

	// 处理成一个报警标志位
	add_map_key("20", to_string(ala), mp);

	char szbuf[512] = { 0 };
	//单独处理附加信息
	if (append_data != NULL && append_data_len > 2) {
		unsigned short cur = 0;
		unsigned char amid = 0;
		unsigned char amlen = 0;
		unsigned short word = 0;
		unsigned int dword = 0;

		while (cur + 2 < append_data_len) {
			word = 0;
			dword = 0;
			amid = append_data[cur];
			amlen = append_data[cur + 1];
			if (cur + 2 + amlen > append_data_len)
				break;
			//printf("amid:%x,amlen:%x \n",amid,amlen);
			switch (amid) {
			case 0x01: //里程
				add_map_key("9", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
				break;
			case 0x02: //油量，WORD�?/10L，对应车上油量表读数
				add_map_key("24", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
				break;
			case 0x03: //行驶记录功能获取的�?度，WORD,1/10KM/h
				add_map_key("7", to_string(get_word(append_data + cur + 2)), mp);
				break;
			case 0x04: // �?��人工确认报警事件的ID，WORD,�?�?��计数
				add_map_key("519", to_string(get_word(append_data + cur + 2)), mp); // 补页新增
				break;
			case 0x11: //超�?报警附加信息
				if (amlen == 1) {
					word = append_data[cur + 2];
					if (word == 0) {
						add_map_key("31", "0|", mp);
					}
				} else if (amlen == 5) {
					word = append_data[cur + 2];
					if (word >= 1 && word <= 4) {
						dword = get_dword(append_data + cur + 3);
						sprintf(szbuf, "%u|%u", word, dword);
						add_map_key("31", szbuf, mp);
					}
				}
				break;
			case 0x12: //进出区域/路线报警附加信息
				if (amlen == 6) {
					word = append_data[cur + 2];
					if (word >= 1 && word <= 4) {
						dword = get_dword(append_data + cur + 3);
						unsigned short d = append_data[cur + 2 + 5];
						if (d == 0 || d == 1) {
							sprintf(szbuf, "%u|%u|%u", word, dword, d);
							add_map_key("32", szbuf, mp);
						}
					}
				}
				break;
			case 0x13: //路段行驶时间不足/过长报警附加信息
				if (amlen == 7) {
					dword = get_dword(append_data + cur + 2);
					word = get_word(append_data + cur + 6);

					unsigned short d = append_data[cur + 8];
					if (d == 0 || d == 1) {
						sprintf(szbuf, "%u|%u|%u", dword, word, d);
						add_map_key("35", szbuf, mp);
					}
				}
				break;
			case 0x14: // �?��工确认的报警流水号，定义见表20-3  4字节
			{
				add_map_key("520", to_string(get_dword(append_data + cur + 2)), mp);
			}
				break;
			case 0xE0: // 后继信息长度
				// ToDo:
				break;
			case 0x20: //发动机转�?
			{
				add_map_key("210", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x21: //瞬时油�?
			{
				add_map_key("216", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x22: // 发动机扭矩百分比
			{
				add_map_key("503", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x23: // 油门踏板位置
			{
				add_map_key("504", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x24: // 扩展车辆报警标志�?
			{
				// �?��协议的扩展标志位
				add_map_key("21", to_string(get_dword(append_data + cur + 2)), mp); //to_string
			}
				break;
			case 0x25: // 扩展车辆信号状�?�?
			{
				add_map_key("500", to_string(get_dword(append_data + cur + 2)), mp);
			}
				break;
			case 0x26: // 累计油�?
			{
				add_map_key("213", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
			}
				break;
			case 0x27: // 0x00：带速开门；0x01区域外开门；0x02：区域内�?��；其他�?保留�?字符
			{
				add_map_key("217", to_string((unsigned char) (*(append_data + cur + 2))), mp);
			}
				break;
			case 0x28: // 0x28 VSS还GPS 车�?来源
			{
				add_map_key("218", to_string((unsigned char) (*(append_data + cur + 2))), mp);
			}
				break;
			case 0x29: // 0x29 计量仪油耗，1bit=0.01L,0=0L
			{
				add_map_key("219", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
			}
				break;
			case 0x2A:
			{
				add_map_key("700", to_string(get_word(append_data + cur + 2)), mp);
			}
				break;
			case 0x2B:
			{
				add_map_key("701", to_string(get_dword(append_data + cur + 2)), mp);
			}
				break;
			case 0x30:
			{
				add_map_key("702", to_string(append_data[cur + 2]), mp);
			}
				break;
			case 0x31:
			{
				add_map_key("703", to_string(append_data[cur + 2]), mp);
			}
				break;
			case 0x40: // 发动机运行�?时长
			{
				add_map_key("505", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
			}
				break;
			case 0x41: // 终端内置电池电压
			{
				add_map_key("506", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x42: // 蓄电池电�?
			{
				add_map_key("507", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x43: // 发动机水�?
			{
				add_map_key("214", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x44: // 机油温度
			{
				add_map_key("508", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x45: // 发动机冷却液温度
			{
				add_map_key("509", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x46: // 进气温度
			{
				add_map_key("510", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x47: // 机油压力
			{
				add_map_key("215", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x48: // 大气压力
			{
				add_map_key("511", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x4A: // 油量数据上传
			{
				unsigned char ntype = (unsigned char) append_data[cur + 2];
				unsigned short noil = get_word(append_data + cur + 3);

				// 正常油量
				if (ntype == 0) {
					sprintf(szbuf, "%u|%u", ntype, noil);
					add_map_key("318", szbuf, mp);
				} else { // 加油或�?漏油
					unsigned short nval = get_word(append_data + cur + 5);
					sprintf(szbuf, "%u|%u|%u|%s|%s", ntype, noil, nval, bcd_2_utc((char*) (append_data + cur + 7)).c_str(), bcd_2_utc((char*) (append_data + cur + 13)).c_str());
					add_map_key("318", szbuf, mp);
				}
				break;
			}
				break;
				// 以下为新增的北斗招标部分协议解析
			case 0xfe: // 信号强度
			{
				unsigned char c = (unsigned char) append_data[cur + 2];
				// 信号强度.卫星�?高四位信号强度，低四位信号强�?
				sprintf(szbuf, "%u.%u|%u", ((c >> 4) & 0x0f), (c & 0x0f), (unsigned char) append_data[cur + 3]);
				add_map_key("401", szbuf, mp);
			}
				break;
			case 0xff: // 自定义状态及模拟量上�?
			{
				// BYTE IO状�?1 | BYTE IO状�?2 | WORD 模拟�? | WORD 模拟�?
				sprintf(szbuf, "%u|%u|%u|%u", (unsigned char) append_data[cur + 2], (unsigned char) append_data[cur + 3], get_word(append_data + cur + 4), get_word(append_data + cur
						+ 6));
				add_map_key("402", szbuf, mp);
			}
				break;
			default:
				break;
			}
			cur += 2 + amlen;

		}
	}
	/***********************************************************************/
	//dest = "{TYPE:0,RET:0," + dest;
	// 重组内部协议数据
	dest += build_map_command(mp);
	//dest += "}";
	/*if ( !astr.empty() )
	 dest += astr ;
	 */
	return dest;
}

// �?��是否为新808协议
bool GBHandler::check_driver( unsigned char *ptr, int len )
{
	if ( len < 7 ) return false ;

	if ( ptr[0] != 0x01 && ptr[0] != 0x02 )
		return false ;

	// bcd[6] 时间�?Y-m-d H:i:S
	if ( ptr[1] < 0x13 || ptr[1] > 0x99 )
		return false ;
	if ( ptr[2] < 0x01 || ptr[2] > 0x12 )
		return false ;
	if ( ptr[3] < 0x01 || ptr[3] > 0x31 )
		return false ;

	// 时分秒的�?��
	if ( ptr[4] > 0x24 || ptr[5] > 0x60 || ptr[6] > 0x60 )
		return false ;

	return true ;
}

// 安全内存拷贝
static char * safe_memncpy( char *dest, const char *src, int len )
{
	if ( src == NULL )
		return NULL ;

	int nsize  = len ;
	int nlen   = (int) strlen( src ) ;
	if ( nlen < len ) {
		nsize = nlen ;
	}

	memset( dest, 0, len ) ;
	memcpy( dest, src, nsize ) ;

	return dest ;
}

// 获取驾驶员身份信�?
bool GBHandler::get_driver_info(const char *buf, int len, DRIVER_INFO &info)
{
	const char *ptr = buf;
	unsigned char nlen = (unsigned char) (*ptr);
	ptr += 1;

	safe_memncpy(info.drivername, ptr, nlen);
	ptr += nlen;
	if (ptr > buf + len) {
		return false;
	}

	safe_memncpy(info.driverid, ptr, 20);
	ptr += 20;
	if (ptr > buf + len) {
		return false;
	}

	safe_memncpy(info.driverorgid, ptr, 40);
	ptr += 40;
	if (ptr > buf + len) {
		return false;
	}

	nlen = (unsigned char) (*ptr);
	ptr += 1;

	safe_memncpy(info.orgname, ptr, nlen);
	ptr += nlen;
	if (ptr > buf + len) {
		return false;
	}
	return true;
}

//获取驾驶员鉴权身份信�? ---临时方案
bool GBHandler::get_driver_info(const char *buf, int buf_len, DRIVER_INFO_NEW808 &dinfo)
{
	if (NULL == buf || buf_len <= 0)
		return false;

	memset(&dinfo, 0x00, sizeof(DRIVER_INFO_NEW808));
	// 状�?,0x01：从业资格证IC卡插入（驾驶员上班）�?x02：从业资格证IC卡拔出（驾驶员下班）�?
	int offset = 0;
	if (offset + 1 > buf_len)
		return false;
	dinfo.state = buf[offset];
	offset++;
	//时间插卡/拔卡时间，YY-MM-DD-hh-mm-ss；以下字段在状�?�?x01时才有效并做填充�?
	if (offset + 6 > buf_len) {
		return false;
	}
	memcpy(dinfo._timer, buf + offset, 6);
	if (dinfo.state == 0x02)
		return true; //拔卡 特殊处理，关�?
	offset += 6;
	// IC卡读取结�?
	if (offset + 1 > buf_len) {
		return false;
	}
	dinfo.result = buf[offset];
	if (0x00 != dinfo.result)
		return true; //IC卡读卡失败特殊夐理，关键

	offset++;
	// 驾驶员��名长�?
	if (offset + 1 > buf_len) {
		return false;
	}
	int namelen = (unsigned char) buf[offset];
	offset++;
	if (offset + namelen > buf_len) {
		return false;
	}
	// 驾驶员���?
	memcpy(dinfo.name, buf + offset, namelen);
	offset += namelen;
	// 从业资格证编�?
	if (offset + 20 > buf_len) {
		return false;
	}
	memcpy(dinfo.certification, buf + offset, 20);
	offset += 20;
	// 发证机构名称长度
	if (offset + 1 > buf_len) {
		return false;
	}
	int agentlen = (unsigned char) buf[offset];
	offset++;
	if (offset + agentlen > buf_len) {
		return false;
	}
	// 发证机构名称
	memcpy(dinfo.agent, buf + offset, agentlen);
	offset += agentlen;
	// 证件有效�?
	if (offset + 4 > buf_len) {
		return false;
	}
	memcpy(dinfo.expire, buf + offset, 4);
	return true;
}

/*
 * buf：消息体
 * buf_len:消息体长�?
 */
string GBHandler::convert_driver_info(char *buf, int buf_len, unsigned char result)
{
	if (NULL == buf || buf_len <= 0)
		return "";

	string sinfo ;
	// 新版本驾驶员身份识别
	if ( check_driver( (unsigned char *)buf, buf_len ) ) {
		DRIVER_INFO_NEW808 info;
		if (!get_driver_info(buf, buf_len, info)) {
			return "";
		}

		sinfo = ",117:";
		sinfo += to_string(info.state);
		sinfo += ",115:";
		sinfo += get_bcd_time((unsigned char*) info._timer);
		//ic card read result
		if (info.state != 0x02) {
			sinfo += ",116:";
			sinfo += to_string(info.result);
			//driver namelen & name
			if (info.result == 0x00) {
				sinfo += ",110:";
				sinfo += info.name;
				sinfo += ",112:";
				sinfo += info.certification;
				sinfo += ",113:";
				sinfo += info.agent;
				sinfo += ",114:";
				std::string tmp;
				bcd_to_str((char*) info.expire, sizeof(info.expire), tmp);
				sinfo += tmp;
			}
		}
	} else {  // 老版�?08驾驶员身份识�?
		DRIVER_INFO info ;
		if ( ! get_driver_info( buf, buf_len, info ) ) {
			return "" ;
		}
		// 驾驶员���?
		sinfo += ",110:" ;
		sinfo += info.drivername;

		//驾驶员身份证编码
		sinfo += ",111:" ;
		sinfo += info.driverid;

		//从业资格证编�?
		sinfo += ",112:" ;
		sinfo +=info.driverorgid ;

		//发证机构名称
		sinfo += ",113:";
		sinfo += info.orgname;
	}
	// 重组数据返回
	return "{TYPE:8,RESULT:" + to_string(result) + sinfo+ "}";
}

// 查表找�?
static bool get_postion_key(unsigned char key, bool bit, char *val)
{
	const static unsigned char flag[] = { 0x01, 0x02, 0x03, 0x11, 0x12, 0x13, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };
	for (int i = 0; i < 22; ++i) {
		if (flag[i] == key) {
			sprintf(val, "%u", ((i + 1) * 2 - ((bit) ? 1 : 0)));
			return true;
		}
	}
	return false;
}

// 将对应的位转换成处理
static void get_postion_set(const char *buf, string &val)
{
	// 处理�?��状�?标记
	for (int i = 0; i < 32; ++i) {
		for (int n = 0; n < 8; ++n) {
			char sbuf[128] = { 0 };
			bool bit = IS_BIT( (unsigned char)buf[i], n );
			if (!get_postion_key((unsigned char) (i * 8 + n + 1), bit, sbuf)) {
				continue;
			}
			if (!val.empty())
				val += "|";
			val += sbuf;
		}
	}
}

// 将对应的位转捣G���?
static void get_flagbyword(unsigned int n, string &val, int size)
{
	char buf[128] = { 0 };
	for (int i = 0; i < size; ++i) {
		sprintf(buf, "%u", ((i + 1) * 2 - ((IS_BIT( n, i )) ? 0 : 1)));
		if (!val.empty())
			val += "|";
		val += buf;
	}
}

//flag 0:读取�?设置
bool GBHandler::convert_get_para(char *buf, int buf_len, string &data)
{
	/***********张鹤高修�?-9*************************************/
	int curn = sizeof(GBheader);

	//unsigned short seq = get_word(buf+curn) ;
	curn += 2;

	unsigned char pnum = (unsigned char) buf[curn++];

	unsigned long pid = 0;
	unsigned char plen = 0;

	map<string, string> mp;

	char pchar[257] = { 0 };
	for (unsigned char i = 0; i < pnum; ++i) {
		pid = get_dword(buf + curn);
		curn += 4;
		plen = (unsigned char) buf[curn++];
		switch (pid) {
		case 0x0001:
			add_map_key("7", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0002:
			add_map_key("100", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0003:
			add_map_key("101", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0004:
			add_map_key("102", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0005:
			add_map_key("103", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0006:
			add_map_key("104", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0007:
			add_map_key("105", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0010: //APN
			add_map_key("3", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0011:
			add_map_key("4", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0012:
			add_map_key("5", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0013:
			add_map_key("0", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0014: //备份APN
			add_map_key("106", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0015:
			add_map_key("107", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0016:
			add_map_key("108", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0017: //备份IP
			add_map_key("109", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0018:
			add_map_key("1", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0019: // 根据内部协议与�A部协议对应关系夐�?
			add_map_key("110", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x001A: //IC卡认证主服务器地�?
			add_map_key("800", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x001B: //IC卡认证主服务器端�?
			add_map_key("801", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x001C:
			add_map_key("802", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x001D:
			add_map_key("803", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0020:
			add_map_key("111", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0021:
			add_map_key("112", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0022:
			add_map_key("113", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0027:
			add_map_key("114", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0028:
			add_map_key("115", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0029:
			add_map_key("116", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x002C:
			add_map_key("117", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x002D:
			add_map_key("118", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x002E:
			add_map_key("119", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x002F:
			add_map_key("120", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0030:
			add_map_key("121", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0031: // 电子围栏半径
			add_map_key("31", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0040: //监控平台电话号码
			add_map_key("10", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0041: //复位电话号码
			add_map_key("122", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0042:
			add_map_key("123", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0043:
			add_map_key("15", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0044:
			add_map_key("124", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0045:
			add_map_key("125", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0046:
			add_map_key("126", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0047:
			add_map_key("127", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0048:
			add_map_key("9", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0049: //监控平台特权短信号码
			add_map_key("141", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0050:
			add_map_key("142", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0051:
			add_map_key("143", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0052:
			add_map_key("144", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0053:
			add_map_key("145", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0054:
			add_map_key("146", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0055:
			add_map_key("128", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0056:
			add_map_key("129", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0057:
			add_map_key("130", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0058:
			add_map_key("131", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0059:
			add_map_key("132", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x005A:
			add_map_key("133", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x005D: // 碰撞报警参数设置
			add_map_key("407", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005E: // 侧翻报警参数设置
			add_map_key("808", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0064: //定时拍照
			add_map_key("809", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0065: //定距拍照
			add_map_key("810", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0070:
			add_map_key("136", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0071:
			add_map_key("137", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0072:
			add_map_key("138", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0073:
			add_map_key("139", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0074:
			add_map_key("140", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0080:
			add_map_key("147", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0081:
			add_map_key("134", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0082:
			add_map_key("135", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0083:
			add_map_key("41", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0084: // 车牌颜色
			add_map_key("42", to_string(buf[curn]), mp);
			break;
		case 0x005B: // 超�?报警预警差�?
			add_map_key("300", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005C: // 特征系数
			add_map_key("301", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005F: // 油箱容量
			add_map_key("304", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0060: // 位置信息汇报附加信息设置
		{
			/**
			 char szbuf[256] = {0} ;
			 char *p   = szbuf ;
			 char *ptr = (char*)( buf+curn ) ;
			 for ( int i = 0; i < 32; ++ i ) {
			 sprintf( p , "%02X" , (unsigned char)ptr[i] ) ;
			 p +=2 ;
			 }
			 add_map_key( "305" , szbuf , mp ) ;
			 */

			string sval;
			char *ptr = (char *) (buf + curn);
			get_postion_set(ptr, sval);
			add_map_key("305", sval, mp);
		}
			break;
		case 0x0061: // 门开关拍照控�?
		{
			/**
			 add_map_key( "306" , to_string(get_dword(buf+curn)) , mp ) ;
			 */
			string sval;
			get_flagbyword(get_dword(buf + curn), sval, 16);
			add_map_key("306", sval, mp);
		}
			break;
		case 0x0062: // 终端外围传感配置
		{
			/**
			 add_map_key( "307" , to_string(get_dword(buf+curn)) , mp ) ;
			 */
			string sval;
			get_flagbyword(get_dword(buf + curn), sval, 15);
			add_map_key("307", sval, mp);
		}
			break;
		case 0x0063: // 盲区补报模式
			add_map_key("308", to_string(get_dword(buf + curn) + 1), mp);
			break;
		case 0x0066: // 速度来源VSS还是GPS �?��位的数据
			add_map_key("187", to_string((unsigned char) (*(buf + curn))), mp);
			break;
		case 0x0067: // ToDo: 驾驶员登陆控制信息，宇�?新增
			add_map_key("190", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0075: // 分辩�?
			add_map_key("309", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0085: // 车牌分类
		{
			char szbuf[128] = { 0 };
			add_map_key("310", get_buffer(buf + curn, szbuf, 12), mp);
		}
			break;
		case 0x0090: // GNSS定位模式
			add_map_key("811", to_string(buf[curn]), mp);
			break;
		case 0x0091: // GNSS波特�?
			add_map_key("402", to_string(buf[curn]), mp);
			break;
		case 0x0092: // GNSS模块详细定位数据输出频率
			add_map_key("403", to_string(buf[curn]), mp);
			break;
		case 0x0093: // GNSS模块详细定位数据采集频率，单位为�?
			add_map_key("404", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0094: // GNSS模块详细定位数据上传方式
			add_map_key("815", to_string(buf[curn]), mp);
			break;
		case 0x0095: // GNSS模块详细定位数据上传设置
			add_map_key("816", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0100: // CAN总线通道1采集时间间隔(ms)�?表示不采�?
			add_map_key("817", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0101: // CAN总线通道1上传时间间隔(s)�?表示不上�?
			add_map_key("818", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0102: // CAN总线通道2采集时间间隔(ms)�?表示不采�?
			add_map_key("819", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0103: // CAN总线通道2上传时间间隔(s)�?表示不上�?
			add_map_key("820", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0110:
		{ // CAN总线ID单独采集设置
			unsigned int pick_inter = get_dword(buf + curn);
			const unsigned char * arr = (const unsigned char *) buf + curn + 4;
			int channel = (arr[0] & 0x80) == 0x80 ? 1 : 0;
			int frametype = (arr[0] & 0x40) == 0x40 ? 1 : 0;
			int pick_way = (arr[0] & 0x20) == 0x20 ? 1 : 0;
			unsigned int canid = (unsigned char) (arr[0] & 0x1F) << 24 | (unsigned char) arr[1] << 16 | (unsigned char) arr[2] << 8 | (unsigned char) arr[3];
			char tmpbuf[256] = { 0x00 };
			sprintf(tmpbuf, "%u|%u|%u|%u|%u", pick_inter, channel, frametype, pick_way, canid);
			add_map_key("821", tmpbuf, mp);
		}
			break;
		default:
			break;
		}
		curn += plen;
		if (curn >= buf_len)
			break;
	}
	data = "{TYPE:0,RET:0," + build_map_command(mp) + "} \r\n";

	return true;
}

// 设置64的数�?
static void set_int64(DataBuffer *pbuf, unsigned int msgid, uint64_t n)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8(8);
	pbuf->writeInt64(n);
}

// 设置32位数�?
static void set_dword(DataBuffer *pbuf, unsigned int msgid, unsigned int dword)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8(4);
	pbuf->writeInt32(dword);

}

static void set_word(DataBuffer *pbuf, unsigned int msgid, unsigned short word)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8(2);
	pbuf->writeInt16(word);
}

static void set_string(DataBuffer *pbuf, unsigned int msgid, const char *data, int nlen)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8((uint8_t) nlen);

	if (nlen > 0) {
		pbuf->writeBytes((void*) data, nlen);
	}
}

static void set_bytes(DataBuffer *pbuf, unsigned int msgid, unsigned char *data, int nlen, int max)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8(max);

	if (nlen >= max) {
		pbuf->writeBytes(data, max);
	} else {
		pbuf->writeBytes(data, nlen);
		pbuf->write_fill(0, max - nlen);
	}
}

// 更新对应位数�?
static int get_dword_by_flag(const string &val)
{
	int dword = 0, nval = 0, npos = 0;
	vector<string> vec;
	strsplit(val, "|", vec);
	for (int i = 0; i < (int) vec.size(); ++i) {
		if (vec[i].empty()) {
			continue;
		}

		nval = atoi(vec[i].c_str());
		npos = (nval - 1) / 2;
		if (npos < 0 || npos > 32) {
			continue;
		}

		if (nval % 2 == 0) {
			S_BIT( dword, npos);
		} else {
			C_BIT( dword, npos);
		}
	}
	return dword;
}

// 构建位置信息设置参数
static void build_postion_set(const string &val, unsigned char b[32])
{
	const static unsigned char flag[] = { 0x01, 0x02, 0x03, 0x11, 0x12, 0x13, 0x20, 0x21,
			0x22, 0x23, 0x24, 0x25, 0x26, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };

	memset(b, 0, 32);

	int nval = 0, npos = 0, nbyte = 0, nbit = 0;

	vector<string> vec;
	strsplit(val, "|", vec);
	for (int i = 0; i < (int) vec.size(); ++i) {
		if (vec[i].empty()) {
			continue;
		}

		// 取�?处理
		nval = atoi(vec[i].c_str());
		npos = (nval - 1) / 2;

		// 判断是否正确�?
		if (npos < 0 || npos > 22) {
			continue;
		}

		nbyte = (flag[npos] - 1) / 8; // 计算落在哪个内存空间
		nbit = (flag[npos] - 1) % 8; // 取得更新哪个�?

		if (nval % 2 != 0) {
			// 设置对应CHAR对象�?
			S_BIT( b[nbyte], nbit);
		}
	}
}

// 将内部协议的参数设置转为外部协议
bool GBHandler::build_param_set(DataBuffer *pbuf, map<string, string> &p_kv_map , unsigned char &pnum)
{
	p_kv_map.erase("TYPE");
	p_kv_map.erase("RETRY");

	if (p_kv_map.empty()) {
		return false;
	}

	string k, v;
	int uskey = 0;

	pnum = 0;

	//用来保持参数列表
	vector<string> vec_v;
	typedef map<string, string>::iterator MapIter;

	for (MapIter p = p_kv_map.begin(); p != p_kv_map.end(); ++p) {
		k = (string) p->first;
		v = (string) p->second;

		if (v.size() > 256 || v.empty() || k.empty())

			continue;

		uskey = atoi(k.c_str());
		switch (uskey) {
		case 0: //IP
			set_string(pbuf, 0x0013, v.c_str(), v.length());
			pnum++;
			break;
		case 1: //tcp port
			set_dword(pbuf, 0x0018, atoi(v.c_str()));
			pnum++;
			break;
		case 2:
			break;
		case 3: //APN
			set_string(pbuf, 0x0010, v.c_str(), v.length());
			pnum++;
			break;
		case 4: //APN username
			set_string(pbuf, 0x0011, v.c_str(), v.length());
			pnum++;
			break;
		case 5: //APN pwd
			set_string(pbuf, 0x0012, v.c_str(), v.length());
			pnum++;
			break;
		case 6:
			break;
		case 7: //心跳间隔
			set_dword(pbuf, 0x0001, atoi(v.c_str()));
			pnum++;
			break;
		case 8: //报警号码
		case 10: //求助号码
		{
			vec_v.clear();
			strsplit(v, "/", vec_v);
			for (int i = 0; i < (int) vec_v.size(); ++i) {
				v = vec_v[i];
				if (v.empty()) {
					continue;
				}
				set_string(pbuf, 0x0040, v.c_str(), v.length());
				++pnum;
			}
		}
			break;
		case 9: //监听号码
		{
			vec_v.clear();
			strsplit(v, "/", vec_v);
			for (int i = 0; i < (int) vec_v.size(); ++i) {
				v = vec_v[i];
				if (v.empty()) {
					continue;
				}
				set_string(pbuf, 0x0048, v.c_str(), v.length());
				++pnum;
			}
		}
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15: //中心短信号码
			set_string(pbuf, 0x0043, v.c_str(), v.length());
			pnum++;
			break;
		case 16:
			break;
		case 17:
			break;
		case 18: //设置GPS数据回传间隔
			set_dword(pbuf, 0x0029, atoi(v.c_str()));
			pnum++;
			break;
		case 19:
			break;
		case 20:
			break;
		case 21:
			break;
		case 22:
			break;
		case 23:
			break;
		case 24:
			break;
		case 25:
			break;
		case 26:
			break;
		case 27:
			break;
		case 28:
			break;
		case 29:
			break;
		case 30:
			break;
		case 31:
			set_word(pbuf, 0x0031, atoi(v.c_str())); // 补页新增电子围栏半径
			++pnum;
			break;
		case 32:
			break;
		case 33:
			break;
		case 34:
			break;
		case 35:
			break;
		case 36:
			break;
		case 37:
			break;
		case 38:
			break;
		case 39:
			break;
		case 40:
			break;
		case 41: // 设置车牌�?
			set_string(pbuf, 0x0083, v.c_str(), v.length());
			++pnum;
			break;
		case 42: // 设置车牌颜色
			set_word(pbuf, 0x0084, atoi(v.c_str()));
			pnum++;
			break;
		case 43:
			break;
		case 70:
			break;
		case 71:
			break;
		case 90:
			break;
		case 100: //TCP消息应答超时时间
			set_dword(pbuf, 0x0002, atoi(v.c_str()));
			pnum++;
			break;
		case 101: // TCP重传次数
			set_dword(pbuf, 0x0003, atoi(v.c_str()));
			pnum++;
			break;
		case 102: //UDP
			set_dword(pbuf, 0x0004, atoi(v.c_str()));
			pnum++;
			break;
		case 103: // UDP重传次数
			set_dword(pbuf, 0x0005, atoi(v.c_str()));
			pnum++;
			break;
		case 104: // SMS应答超时时间
			set_dword(pbuf, 0x0006, atoi(v.c_str()));
			pnum++;
			break;
		case 105: // SMS重传次数
			set_dword(pbuf, 0x0007, atoi(v.c_str()));
			pnum++;
			break;
		case 106: //备份APN
			set_string(pbuf, 0x0014, v.c_str(), v.length());
			pnum++;
			break;
		case 107:
			set_string(pbuf, 0x0015, v.c_str(), v.length());
			pnum++;
			break;
		case 108:
			set_string(pbuf, 0x0016, v.c_str(), v.length());
			pnum++;
			break;
		case 109: //备份服务器IP
			set_string(pbuf, 0x0017, v.c_str(), v.length());
			pnum++;
			break;
		case 110: //服务器UDP端口
			set_dword(pbuf, 0x0019, atoi(v.c_str()));
			pnum++;
			break;
		case 111: //汇报策略
			set_dword(pbuf, 0x0020, atoi(v.c_str()));
			pnum++;
			break;
		case 112: // 位置汇报
			set_dword(pbuf, 0x0021, atoi(v.c_str()));
			pnum++;
			break;
		case 113:
			set_dword(pbuf, 0x0022, atoi(v.c_str()));
			pnum++;
			break;
		case 114: //休眠时位置汇报时间间�?
			set_dword(pbuf, 0x0027, atoi(v.c_str()));
			pnum++;
			break;
		case 115: // 紧�?报警时汇报时间间�?
			set_dword(pbuf, 0x0028, atoi(v.c_str()));
			pnum++;
			break;
		case 116: //
			set_dword(pbuf, 0x0029, atoi(v.c_str()));
			pnum++;
			break;
		case 117: //缺省距�a汇报间隔，单位为米（m），>0
			set_dword(pbuf, 0x002C, atoi(v.c_str()));
			pnum++;
			break;
		case 118:
			set_dword(pbuf, 0x002D, atoi(v.c_str()));
			pnum++;
			break;
		case 119:
			set_dword(pbuf, 0x002E, atoi(v.c_str()));
			pnum++;
			break;
		case 120:
			set_dword(pbuf, 0x002F, atoi(v.c_str()));
			pnum++;
			break;
		case 121:
			set_dword(pbuf, 0x0030, atoi(v.c_str()));
			pnum++;
			break;
		case 122: //复位电话号码，可采用此电话号码拨打终端电话让终端复位
			set_string(pbuf, 0x0041, v.c_str(), v.length());
			pnum++;
			break;
		case 123: //恢夙出厂设置电话号码，可采用此电话号码拨打终端电话让终端恢夙出厂设置
			set_string(pbuf, 0x0042, v.c_str(), v.length());
			pnum++;
			break;
		case 124: // 接收终端SMS文本报警号码
			set_string(pbuf, 0x0044, v.c_str(), v.length());
			pnum++;
			break;
		case 125: // 终端电话接听策略
			set_dword(pbuf, 0x0045, atoi(v.c_str()));
			pnum++;
			break;
		case 126: // 每次�?��通话时间
			set_dword(pbuf, 0x0046, atoi(v.c_str()));
			pnum++;
			break;
		case 127: // 当月�?��通话时间
			set_dword(pbuf, 0x0047, atoi(v.c_str()));
			pnum++;
			break;
		case 128: //�?��时�?
			set_dword(pbuf, 0x0055, atoi(v.c_str()));
			pnum++;
			break;
		case 129:
			set_dword(pbuf, 0x0056, atoi(v.c_str()));
			pnum++;
			break;
		case 130:
			set_dword(pbuf, 0x0057, atoi(v.c_str()));
			pnum++;
			break;
		case 131: //当天累计驾驶时间门限，单位为秒（s�?
			set_dword(pbuf, 0x0058, atoi(v.c_str()));
			pnum++;
			break;
		case 132:
			set_dword(pbuf, 0x0059, atoi(v.c_str()));
			pnum++;
			break;
		case 133:
			set_dword(pbuf, 0x005A, atoi(v.c_str()));
			pnum++;
			break;
		case 134: //车辆�?��省域ID
			set_word(pbuf, 0x0081, atoi(v.c_str()));
			pnum++;
			break;
		case 135:
			set_word(pbuf, 0x0082, atoi(v.c_str()));
			pnum++;
			break;
		case 136: //图像/视频质量-1�?0�?�?��;
			set_dword(pbuf, 0x0070, atoi(v.c_str()));
			pnum++;
			break;
		case 137:
			set_dword(pbuf, 0x0071, atoi(v.c_str()));
			pnum++;
			break;
		case 138:
			set_dword(pbuf, 0x0072, atoi(v.c_str()));
			pnum++;
			break;
		case 139:
			set_dword(pbuf, 0x0073, atoi(v.c_str()));
			pnum++;
			break;
		case 140:
			set_dword(pbuf, 0x0074, atoi(v.c_str()));
			pnum++;
			break;
		case 141: //监管平台特权短信号码
			set_string(pbuf, 0x0049, v.c_str(), v.length());
			pnum++;
			break;
		case 142: //报警屏蔽�?
			set_dword(pbuf, 0x0050, atoi(v.c_str()));
			pnum++;
			break;
		case 143: //报警发�?文本�?��SMS�?��
			set_dword(pbuf, 0x0051, atoi(v.c_str()));
			pnum++;
			break;
		case 144: //报警拍摄�?��
			set_dword(pbuf, 0x0052, atoi(v.c_str()));
			pnum++;
			break;
		case 145: //报警拍摄存储标志
			set_dword(pbuf, 0x0053, atoi(v.c_str()));
			pnum++;
			break;
		case 146: //关键报警标志
			set_dword(pbuf, 0x0054, atoi(v.c_str()));
			pnum++;
			break;
		case 147: //车辆里程表读�?
			set_dword(pbuf, 0x0080, atoi(v.c_str()));
			pnum++;
			break;
		case 180: //定时拍照
			set_dword(pbuf, 0x0064, atoi(v.c_str()));
			pnum++;
			break;
		case 181: //定距拍照
			set_dword(pbuf, 0x0065, atoi(v.c_str()));
			pnum++;
			break;
		case 187: // 设置VSS速度优先还是GPS优先
			set_word(pbuf, 0x0066, atoi(v.c_str()));
			++pnum;
			break;
		case 190: // ToDo: 设置驾驶员登陆拍照控�?宇�?新增
			set_word(pbuf, 0x0067, atoi(v.c_str()));
			++pnum;
			break;
		case 300: // 超�?报警预警差�? WORD
			set_word(pbuf, 0x005B, atoi(v.c_str()));
			++pnum;
			break;
		case 301: // 疲劳驾驶预警差�?
			set_word(pbuf, 0x005C, atoi(v.c_str()));
			++pnum;
			break;
		case 302: // 特征系数
			set_word(pbuf, 0x005D, atoi(v.c_str()));
			++pnum;
			break;
		case 303: // 车轮每转脉冲�?
			set_word(pbuf, 0x005E, atoi(v.c_str()));
			++pnum;
			break;
		case 304: // 油箱容量
			set_word(pbuf, 0x005F, atoi(v.c_str()));
			++pnum;
			break;
		case 305: // 位置信息汇报附加设置
		{
			unsigned char b[32] = { 0 };
			build_postion_set(v, b);
			set_bytes(pbuf, 0x0060, b, 32, 32);
			++pnum;
		}
			break;
		case 306: // 门开关拍照控�?
			set_dword(pbuf, 0x0061, get_dword_by_flag(v));
			++pnum;
			break;
		case 307: // 终端外围传感配置
			set_dword(pbuf, 0x0062, get_dword_by_flag(v));
			++pnum;
			break;
		case 308: // 盲区补报模式
			set_dword(pbuf, 0x0063, atoi(v.c_str()) - 1);
			++pnum;
			break;
		case 309: // 分辩�?
			set_word(pbuf, 0x0075, atoi(v.c_str()));
			++pnum;
			break;
		case 310: // 车牌分类
			set_bytes(pbuf, 0x0085, (unsigned char*) v.c_str(), v.length(), 12);
			++pnum;
			break;
		case 800: // 道路运输证IC 卡认证主服务器IP 地址或域�?
			set_string(pbuf, 0x001A, v.c_str(), v.length());
			++pnum;
			break;
		case 801: // 道路运输证IC 卡认证主服务器TCP 端口
			set_dword(pbuf, 0x001B, atoi(v.c_str()));
			++pnum;
			break;
		case 802: // 道路运输证IC 卡认证主服务器UDP 端口
			set_dword(pbuf, 0x001C, atoi(v.c_str()));
			++pnum;
			break;
		case 803: // 道路运输证IC 卡认证夓份服务器IP 地址或域名，端口同主服务�?
			set_string(pbuf, 0x001D, v.c_str(), v.length());
			++pnum;
			break;
		/**
		case 805: // 超�?报警预警差�?，单位为1/10Km/h
			set_word(pbuf, 0x005B, atoi(v.c_str()));
			++pnum;
			break;
		case 806: // 疲劳驾驶预警差�?，单位为秒（s），>0
			set_word(pbuf, 0x005C, atoi(v.c_str()));
			++pnum;
			break;
		*/
		case 407: // 侧翻报警参数设置：侧翻��度，单位1 度，默认�?0 �?
			set_word(pbuf, 0x005D, atoi(v.c_str()));
			++pnum;
			break;
		case 808: // 碰撞报警参数设置
			set_word(pbuf, 0x005E, atoi(v.c_str()));
			++pnum;
			break;
		case 809: // 定时拍照控制
			set_dword(pbuf, 0x0064, atoi(v.c_str()));
			++pnum;
			break;
		case 810: // 定距拍照控制
			set_dword(pbuf, 0x0065, atoi(v.c_str()));
			++pnum;
			break;
		case 811: // GNSS 定位模式
			set_word(pbuf, 0x0090, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 402: // GNSS 波特�?
			set_word(pbuf, 0x0091, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 403: // GNSS 模块详细定位数据输出频率
			set_word(pbuf, 0x0092, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 404: // GNSS采集NMEA数据频率，单位为�?
			set_dword(pbuf, 0x0093, atoi(v.c_str()));
			++pnum;
			break;
		case 815: // GNSS 模块详细定位数据上传方式
			set_word(pbuf, 0x0094, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 816: // GNSS 模块详细定位数据上传设置
			set_dword(pbuf, 0x0095, atoi(v.c_str()));
			++pnum;
			break;
		case 817: // CAN 总线通道1 采集时间间隔(ms)�? 表示不采�?
			set_dword(pbuf, 0x0100, atoi(v.c_str()));
			++pnum;
			break;
		case 818: // CAN 总线通道1 上传时间间隔(s)�? 表示不上�?
			set_word(pbuf, 0x0101, atoi(v.c_str()));
			++pnum;
			break;
		case 819: // CAN 总线通道2 采集时间间隔(ms)�? 表示不采�?
			set_dword(pbuf, 0x0102, atoi(v.c_str()));
			++pnum;
			break;
		case 820: // CAN 总线通道2 上传时间间隔(s)�? 表示不上�?
			set_word(pbuf, 0x0103, atoi(v.c_str()));
			++pnum;
			break;
		case 821: // CAN 总线ID 单独采集设置
		{
			// 采集时间间隔|CAN 通道号|帧类型|数据采集方式|CAN 总线ID
			unsigned int pick_inter = 0, channel = 0, frametype = 0, pick_way = 0, canid = 0;
			sscanf(v.c_str(), "%u|%u|%u|%u|%u", &pick_inter, &channel, &frametype, &pick_way, &canid);
			// 转成64的数�?
			uint64_t n = 0;
			// 设置CAN通道 �?
			if (channel > 0)   S_BIT( n, 31 );
			// 表示帧类�?
			if (frametype > 0) S_BIT( n, 30 );
			// 表示数据采集方式
			if (pick_way > 0)  S_BIT( n, 29 );
			// 表示CAN总线ID�?9位数�?
			n = ( ( n | (canid & 0x1fff)) & 0x00000000ffffffffLL ) ;
			// 将高32位设置为采集时间间隔
			n |= (((unsigned long long) pick_inter << 32) & 0xffffffff00000000LL);
			// 设置终端参数
			set_int64(pbuf, 0x0110, n);

			++pnum;
		}
			break;
		default:
			break;
		}
	}

	return true;
}

unsigned char GBHandler::get_check_sum(const char *buf, int len)
{
	if (buf == NULL || len < 1)
		return 0;
	unsigned char check_sum = 0;
	for (int i = 0; i < len; i++) {
		check_sum ^= buf[i];
	}
	return check_sum;
}

string GBHandler::get_bcd_time(unsigned char bcd[6])
{
	string dest;
	char buf[4] = { 0 };

	sprintf(buf, "%02x", bcd[0]);

	// 20110308/150234
	unsigned int year = atoi(buf) + 2000;
	dest += to_string(year);

	// memset(buf,0,4);
	sprintf(buf, "%02x", bcd[1]);
	dest += buf;

	sprintf(buf, "%02x", bcd[2]);
	dest += buf;
	dest += "/";

	sprintf(buf, "%02x", bcd[3]);
	dest += buf;
	sprintf(buf, "%02x", bcd[4]);
	dest += buf;
	sprintf(buf, "%02x", bcd[5]);
	dest += buf;
	return dest;
}

//20110304
string GBHandler::get_date()
{
	string str_date;
	char date[128] = { 0 };
	time_t t;
	time(&t);
	struct tm local_tm;
	struct tm *tm = localtime_r(&t, &local_tm);

	sprintf(date, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
	str_date = date;
	return str_date;
}

//050507
string GBHandler::get_time()
{
	string str_time;
	char time1[128] = { 0 };

	time_t t;
	time(&t);
	struct tm local_tm;
	struct tm *tm = localtime_r(&t, &local_tm);

	sprintf(time1, "%04d%02d%02d%02d%02d%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	str_time = time1;
	return str_time;

}

// 添加北斗招标的扩展消息上报协�?
bool GBHandler::convert_report(const char *pbuf, int len, string &data)
{
	// BYTE
	unsigned char size = *pbuf;
	if (size == 0)
		return false;

	char *p = (char *) (pbuf + 1);

	char szbuf[512] = { 0 };

	unsigned char n = 0, i = 0;
	unsigned int id = 0;
	int offset = 0;

	// DWORD BYTE DATALENGTH
	for (i = 0; i < size; ++i) {
		if (offset >= len)
			break;
		id = get_dword(p + offset);
		offset += 4;
		n = (unsigned char) p[offset];
		offset += 1;

		switch (id) {
		case 0x0001: // 终端属�?上报
		{
			// 终端类型 | 终端版本�?0x0207,表示2.07版本) | GNSS模块属�? | 通信模块属�? | 终端属�?
			sprintf(szbuf, ",601:%u|%u|%u|%u|%u",
					get_dword(p + offset), get_dword(p + offset + 4),
					get_dword(p + offset + 8), get_dword(p + offset + 12), get_dword(p + offset + 16));
			data += szbuf;
		}
			break;
		case 0x0002: // 扩展文本信息上发指令
		{
			// 标志位｜文本信息(base64)
			unsigned char type = (unsigned char) p[offset];
			if (n > 1) {
				Base64 coder;
				coder.encode(p + offset + 1, n - 1);
				sprintf(szbuf, ",602:%u|%s", type, coder.data());
				data += szbuf;
			}
		}
			break;
		}
		offset += n;
	}

	return true;
}

// 终端升级应答
bool GBHandler::convert_term_upgrade_result_notify(const char * buf, int len, char * datap)
{
	if (buf == NULL || len < (int) sizeof(DownUpgradeResult))
		return false;
	// 终端升级结果
	DownUpgradeResult *rsp = (DownUpgradeResult *) (buf);
	// 组建内部协议
	sprintf(datap, ",707:%d|%d", rsp->type, rsp->result);

	return true;
}

// 添加 �?08 查询终端属�?应答上报
bool GBHandler::convert_report_new808(const char *buf, int len, char * datap)
{
	int offset = (int) sizeof(TermAttrib);
	if (buf == NULL || len < offset + 2)
		return false;

	// 主动上报的终端参数属�?
	TermAttrib *attr = (TermAttrib *) buf;
	// 终端属�?类型
	unsigned short type = ntohs(attr->type) ;
	// 制�?商ID
	char manuid[6] = { 0 };
	get_buffer((char*) attr->corpid, manuid, sizeof(attr->corpid));
	// 终端类型
	char term_model[21] = { 0 };
	get_buffer((const char*) attr->termtype, term_model, sizeof(attr->termtype));
	// 终端ID
	char term_id[8] = { 0 };
	get_buffer((const char*) attr->termid, term_id, sizeof(attr->termid));
	// 终端SIM卡ICCID
	string iccid;
	bcd_to_str((char *) attr->iccid, sizeof(attr->iccid), iccid);

	// 终端硬件版本号长�?
	unsigned char nhver = (unsigned char) buf[offset];
	offset = offset + 1;

	// 终端硬件版本�?
	char hver[257] = { 0 };
	if (nhver > 0)
		get_buffer((const char*) (buf + offset), hver, nhver);
	offset = offset + nhver;

	if (offset > len)
		return false;

	// 终端固件版本号长�?
	unsigned char nfver = (unsigned char) buf[offset];
	offset = offset + 1;

	// 终端固件版本�?
	char fver[257] = { 0 };
	if (nfver > 0)
		get_buffer((const char*) (buf + offset), fver, nfver);
	offset = offset + nfver;

	if (offset > len)
		return false;

	unsigned char gnss_property = (unsigned char) buf[offset];
	unsigned char comm_property = (unsigned char) buf[offset + 1];

	sprintf(datap, ",706:%d|%s|%s|%s|%s|%s|%s|%d|%d",
			type, manuid, term_model, term_id, iccid.c_str(), hver, fver, gnss_property, comm_property);

	return true;
}

// 构建参数查询处理
bool GBHandler::convert_get_param2(const char *pbuf, int len, string &data)
{
	unsigned char size = *pbuf;
	if (size == 0)
		return false;

	char *p = (char *) (pbuf + 1);

	char szbuf[512] = { 0 };

	unsigned char n = 0, i = 0;
	unsigned int id = 0;
	int offset = 0;

	// DWORD BYTE DATALENGTH
	for (i = 0; i < size; ++i) {
		if (offset >= len)
			break;
		id = get_dword(p + offset);
		offset += 4;
		n = (unsigned char) p[offset];
		offset += 1;

		switch (id) {
		case 0x0001: // GNSS定位模式切换
		case 0x0002: // GNSS波特率设�?
		case 0x0003: // GNSS NMEA输出更新率设�?
		case 0x0004: // GNSS采集NMEA数据频率
		case 0x0005: // CAN1参数设置
		case 0x0006: // CAN2参数设置
		case 0x0007: // 碰撞参数设置
		{
			sprintf(szbuf, ",%u:%u", (400 + id), get_dword(p + offset));
			data += szbuf;
		}
			break;
		}
		offset += n;
	}

	return true;
}

// 解析下发参数设置
bool GBHandler::build_set_param2(DataBuffer &buf, map<string, string> &mp , unsigned char &num)
{
	if (mp.empty())
		return false;

	unsigned int flag = 0;
	map<string, string>::iterator it;
	for (it = mp.begin(); it != mp.end(); ++it) {
		flag = atoi(it->first.c_str());
		switch (flag) {
		case 401:
		case 402:
		case 403:
		case 404:
		case 405:
		case 406:
		case 407:
		{
			buf.writeInt32(flag - 400);
			buf.writeInt8(sizeof(int));
			buf.writeInt32(atoi(it->second.c_str()));

			num = num + 1;
		}
			break;
		}
	}
	return (num > 0);
}

// 解析带扩号标识的数据
static bool parse_vector(const string &sval, vector<string> &vec, const char cbegin, const char cend)
{
	size_t end = 0;
	size_t pos = sval.find(cbegin, 0);
	while (pos != string::npos) {
		end = sval.find(cend, pos + 1);
		if (end == string::npos) {
			break;
		}
		// 存放解析出来的数据[]分割数据
		vec.push_back(sval.substr(pos + 1, end - pos - 1));
		pos = sval.find(cbegin, end + 1);
	}

	if (vec.empty()) {
		return false;
	}

	return true;
}

// 解析扩展参数设置2
bool GBHandler::build_set_param_2ex(vector<DataBuffer*> &vbuf, map<string, string> &mp)
{
	if (mp.empty())
		return false;

	unsigned int flag = 0;
	map<string, string>::iterator it;
	for (it = mp.begin(); it != mp.end(); ++it) {
		flag = atoi(it->first.c_str());
		switch (flag) {
		case 501: // CAN ID参数设置
		{
			// 操作｜[CAN1设置项][CAN2设置项][CAN3设置项]
			string &s = it->second;
			size_t pos = s.find('|');
			if (pos == string::npos) {
				continue;
			}

			vector<string> vec;
			if (!parse_vector(s.substr(pos + 1), vec, '[', ']')) {
				continue;
			}
			unsigned char op = atoi(s.substr(0, pos).c_str());

			int size = vec.size();

			int num = 0;
			DataBuffer dbuf;
			// ID号｜ID属�?｜ID值｜ID页属性｜ID采集间隔时间
			for (int i = 0; i < size; ++i) {
				vector<string> tmp;
				strsplit(vec[i], "|", tmp);
				if (tmp.size() != 5) {
					continue;
				}
				dbuf.writeInt8(atoi(tmp[0].c_str())); // ID�?
				dbuf.writeInt8(atoi(tmp[1].c_str())); // ID属�?
				dbuf.writeInt32(atoi(tmp[2].c_str())); // ID�?
				dbuf.writeInt8(atoi(tmp[3].c_str())); // ID页属�?
				dbuf.writeInt16(atoi(tmp[4].c_str())); // ID采集间隔时间

				num = num + 1;

				if (dbuf.getDataLen() + 11 > 255) {
					DataBuffer *ptmp = new DataBuffer;
					ptmp->writeInt16(0x0001);
					ptmp->writeInt8(dbuf.getDataLen() + 2);
					ptmp->writeInt8(op); // 设置操作
					ptmp->writeInt8(num); // 个数
					ptmp->writeBytes(dbuf.getData(), dbuf.getDataLen());
					vbuf.push_back(ptmp);
					dbuf.clear();
					num = 0;
				}
			}

			// 处理�?���?��数据
			if (dbuf.getDataLen() > 0) {
				DataBuffer *ptmp = new DataBuffer;
				ptmp->writeInt32(0x0001); // 将WORD改成DWORD
				ptmp->writeInt8(dbuf.getDataLen() + 2);
				ptmp->writeInt8(op); // 设置操作
				ptmp->writeInt8(num); // 个数
				ptmp->writeBytes(dbuf.getData(), dbuf.getDataLen());
				vbuf.push_back(ptmp);
			}
		}
			break;
		case 502: // 扩展文本下发设置
		{
			// 保留（默�?）｜标志｜文本信�?base64)
			vector<string> vec;
			strsplit(it->second, "|", vec);
			if (vec.size() != 3) {
				continue;
			}
			// 如果下发文本为空
			if (vec[2].empty()) {
				continue;
			}

			Base64 coder;
			if (!coder.decode(vec[2].c_str(), vec[2].length())) {
				continue;
			}
			if (coder.size() > 255) {
				continue;
			}

			DataBuffer *pbuf = new DataBuffer;
			pbuf->writeInt32(0x0002); // 将WORD改成DWORD
			pbuf->writeInt8(coder.size() + 2);
			pbuf->writeInt8(0);
			pbuf->writeInt8(atoi(vec[1].c_str()));
			pbuf->writeBytes(coder.data(), coder.size());

			vbuf.push_back(pbuf);
		}
			break;
		}
	}
	return (!vbuf.empty());
}

// 转换成十六进�?
void GBHandler::print_hex(unsigned char *p, int n, char *buf)
{
	for (int i = 0; i < n; ++i) {
		sprintf(buf + i * 2, "%02x", p[i]);
	}
}

} // namespace triones
