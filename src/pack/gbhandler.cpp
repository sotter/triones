/**
 * author: Triones
 * date  : 2014-09-03
 */

#include <arpa/inet.h>
#include "../utils/base64.h"
#include "../utils/tools.h"
#include "../utils/str.h"
#include "../log/comlog.h"
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

// 灏嗗瓧绗︿覆鏃堕棿杞负BCD鏃堕棿
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

	//鍒濆鍖栭�鐢ㄥ洖澶嶆秷鎭ご
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

// 杞崲椹鹃┒琛屼负浜嬩欢
string GBHandler::convert_event_gps(GpsInfo *gps)
{
	string dest;
	if (gps == NULL)
		return dest;

	// [璧峰浣嶇疆绾害][璧峰浣嶇疆缁忓害][璧峰浣嶇疆楂樺害][璧峰浣嶇疆閫熷害][璧峰浣嶇疆鏂瑰悜][璧峰浣嶇疆鏃堕棿]
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

// 鏋勫缓MAP鐨凨EY鍜孷ALUE鍊肩殑鍏崇郴
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

	//姝ｅ寳鏂瑰悜涓�锛岄『鏃堕拡鏂瑰悜锛屽崟浣嶄负2搴︺�
	add_map_key("5", to_string(ntohs(gps_info->direction)), mp);
	add_map_key("6", to_string(ntohs(gps_info->heigth)), mp);

	// 娣诲姞褰撳墠鎺ユ敹鏃堕棿
	char sz[128] = {0};
	snprintf( sz, sizeof(sz)-1, "%lu", (long)time(NULL) ) ;
	add_map_key("999", sz , mp ) ;

	/*寮犻工楂樺鍔�***********************************************************/
	//DWORD,浣嶇疆鍩烘湰淇℃伅鐘舵�浣嶏紝B0~B15,鍙傝�JT/T808-2011,Page15锛岃〃17
	unsigned int status = 0;
	memcpy(&status, &(gps_info->state), sizeof(unsigned int));
	status = ntohl(status);
	// 澶勭悊鐘舵�
	add_map_key("8", to_string(status), mp);

	//瑙ｆ瀽鎶ヨ鏍囧織浣�
	int ala = 0;
	memcpy(&ala, &(gps_info->alarm), sizeof(int));
	ala = ntohl(ala);

	// 澶勭悊鎴愪竴涓姤璀︽爣蹇椾綅
	add_map_key("20", to_string(ala), mp);

	char szbuf[512] = { 0 };
	//鍗曠嫭澶勭悊闄勫姞淇℃伅
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
			case 0x01: //閲岀▼
				add_map_key("9", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
				break;
			case 0x02: //娌归噺锛學ORD锛�/10L锛屽搴旇溅涓婃补閲忚〃璇绘暟
				add_map_key("24", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
				break;
			case 0x03: //琛岄┒璁板綍鍔熻兘鑾峰彇鐨勯�搴︼紝WORD,1/10KM/h
				add_map_key("7", to_string(get_word(append_data + cur + 2)), mp);
				break;
			case 0x04: // 闇�浜哄伐纭鎶ヨ浜嬩欢鐨処D锛學ORD,浠�寮�璁℃暟
				add_map_key("519", to_string(get_word(append_data + cur + 2)), mp); // 琛ラ〉鏂板
				break;
			case 0x11: //瓒呴�鎶ヨ闄勫姞淇℃伅
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
			case 0x12: //杩涘嚭鍖哄煙/璺嚎鎶ヨ闄勫姞淇℃伅
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
			case 0x13: //璺琛岄┒鏃堕棿涓嶈冻/杩囬暱鎶ヨ闄勫姞淇℃伅
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
			case 0x14: // 闇�汉宸ョ‘璁ょ殑鎶ヨ娴佹按鍙凤紝瀹氫箟瑙佽〃20-3  4瀛楄妭
			{
				add_map_key("520", to_string(get_dword(append_data + cur + 2)), mp);
			}
				break;
			case 0xE0: // 鍚庣户淇℃伅闀垮害
				// ToDo:
				break;
			case 0x20: //鍙戝姩鏈鸿浆閫�
			{
				add_map_key("210", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x21: //鐬椂娌硅�
			{
				add_map_key("216", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x22: // 鍙戝姩鏈烘壄鐭╃櫨鍒嗘瘮
			{
				add_map_key("503", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x23: // 娌归棬韪忔澘浣嶇疆
			{
				add_map_key("504", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x24: // 鎵╁睍杞﹁締鎶ヨ鏍囧織浣�
			{
				// 鏈�柊鍗忚鐨勬墿灞曟爣蹇椾綅
				add_map_key("21", to_string(get_dword(append_data + cur + 2)), mp); //to_string
			}
				break;
			case 0x25: // 鎵╁睍杞﹁締淇″彿鐘舵�浣�
			{
				add_map_key("500", to_string(get_dword(append_data + cur + 2)), mp);
			}
				break;
			case 0x26: // 绱娌硅�
			{
				add_map_key("213", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
			}
				break;
			case 0x27: // 0x00锛氬甫閫熷紑闂紱0x01鍖哄煙澶栧紑闂紱0x02锛氬尯鍩熷唴寮�棬锛涘叾浠栧�淇濈暀锛�瀛楃
			{
				add_map_key("217", to_string((unsigned char) (*(append_data + cur + 2))), mp);
			}
				break;
			case 0x28: // 0x28 VSS杩楪PS 杞﹂�鏉ユ簮
			{
				add_map_key("218", to_string((unsigned char) (*(append_data + cur + 2))), mp);
			}
				break;
			case 0x29: // 0x29 璁￠噺浠补鑰楋紝1bit=0.01L,0=0L
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
			case 0x40: // 鍙戝姩鏈鸿繍琛屾�鏃堕暱
			{
				add_map_key("505", i_to_decstr(get_dword(append_data + cur + 2), MAX_DWORD_INVALID, false), mp);
			}
				break;
			case 0x41: // 缁堢鍐呯疆鐢垫睜鐢靛帇
			{
				add_map_key("506", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x42: // 钃勭數姹犵數鍘�
			{
				add_map_key("507", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x43: // 鍙戝姩鏈烘按娓�
			{
				add_map_key("214", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x44: // 鏈烘补娓╁害
			{
				add_map_key("508", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x45: // 鍙戝姩鏈哄喎鍗存恫娓╁害
			{
				add_map_key("509", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x46: // 杩涙皵娓╁害
			{
				add_map_key("510", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, false), mp);
			}
				break;
			case 0x47: // 鏈烘补鍘嬪姏
			{
				add_map_key("215", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x48: // 澶ф皵鍘嬪姏
			{
				add_map_key("511", i_to_decstr(get_word(append_data + cur + 2), MAX_WORD_INVALID, true), mp);
			}
				break;
			case 0x4A: // 娌归噺鏁版嵁涓婁紶
			{
				unsigned char ntype = (unsigned char) append_data[cur + 2];
				unsigned short noil = get_word(append_data + cur + 3);

				// 姝ｅ父娌归噺
				if (ntype == 0) {
					sprintf(szbuf, "%u|%u", ntype, noil);
					add_map_key("318", szbuf, mp);
				} else { // 鍔犳补鎴栬�婕忔补
					unsigned short nval = get_word(append_data + cur + 5);
					sprintf(szbuf, "%u|%u|%u|%s|%s", ntype, noil, nval, bcd_2_utc((char*) (append_data + cur + 7)).c_str(), bcd_2_utc((char*) (append_data + cur + 13)).c_str());
					add_map_key("318", szbuf, mp);
				}
				break;
			}
				break;
				// 浠ヤ笅涓烘柊澧炵殑鍖楁枟鎷涙爣閮ㄥ垎鍗忚瑙ｆ瀽
			case 0xfe: // 淇″彿寮哄害
			{
				unsigned char c = (unsigned char) append_data[cur + 2];
				// 淇″彿寮哄害.鍗槦鏁�楂樺洓浣嶄俊鍙峰己搴︼紝浣庡洓浣嶄俊鍙峰己搴�
				sprintf(szbuf, "%u.%u|%u", ((c >> 4) & 0x0f), (c & 0x0f), (unsigned char) append_data[cur + 3]);
				add_map_key("401", szbuf, mp);
			}
				break;
			case 0xff: // 鑷畾涔夌姸鎬佸強妯℃嫙閲忎笂浼�
			{
				// BYTE IO鐘舵�1 | BYTE IO鐘舵�2 | WORD 妯℃嫙閲� | WORD 妯℃嫙閲�
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
	// 閲嶇粍鍐呴儴鍗忚鏁版嵁
	dest += build_map_command(mp);
	//dest += "}";
	/*if ( !astr.empty() )
	 dest += astr ;
	 */
	return dest;
}

// 妫�祴鏄惁涓烘柊808鍗忚
bool GBHandler::check_driver( unsigned char *ptr, int len )
{
	if ( len < 7 ) return false ;

	if ( ptr[0] != 0x01 && ptr[0] != 0x02 )
		return false ;

	// bcd[6] 鏃堕棿锛�Y-m-d H:i:S
	if ( ptr[1] < 0x13 || ptr[1] > 0x99 )
		return false ;
	if ( ptr[2] < 0x01 || ptr[2] > 0x12 )
		return false ;
	if ( ptr[3] < 0x01 || ptr[3] > 0x31 )
		return false ;

	// 鏃跺垎绉掔殑妫�祴
	if ( ptr[4] > 0x24 || ptr[5] > 0x60 || ptr[6] > 0x60 )
		return false ;

	return true ;
}

// 瀹夊叏鍐呭瓨鎷疯礉
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

// 鑾峰彇椹鹃┒鍛樿韩浠戒俊鎭�
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

//鑾峰彇椹鹃┒鍛橀壌鏉冭韩浠戒俊鎭� ---涓存椂鏂规
bool GBHandler::get_driver_info(const char *buf, int buf_len, DRIVER_INFO_NEW808 &dinfo)
{
	if (NULL == buf || buf_len <= 0)
		return false;

	memset(&dinfo, 0x00, sizeof(DRIVER_INFO_NEW808));
	// 鐘舵�,0x01锛氫粠涓氳祫鏍艰瘉IC鍗℃彃鍏ワ紙椹鹃┒鍛樹笂鐝級锛�x02锛氫粠涓氳祫鏍艰瘉IC鍗℃嫈鍑猴紙椹鹃┒鍛樹笅鐝級銆�
	int offset = 0;
	if (offset + 1 > buf_len)
		return false;
	dinfo.state = buf[offset];
	offset++;
	//鏃堕棿鎻掑崱/鎷斿崱鏃堕棿锛孻Y-MM-DD-hh-mm-ss锛涗互涓嬪瓧娈靛湪鐘舵�涓�x01鏃舵墠鏈夋晥骞跺仛濉厖銆�
	if (offset + 6 > buf_len) {
		return false;
	}
	memcpy(dinfo._timer, buf + offset, 6);
	if (dinfo.state == 0x02)
		return true; //鎷斿崱 鐗规畩澶勭悊锛屽叧閿�
	offset += 6;
	// IC鍗¤鍙栫粨鏋�
	if (offset + 1 > buf_len) {
		return false;
	}
	dinfo.result = buf[offset];
	if (0x00 != dinfo.result)
		return true; //IC鍗¤鍗″け璐ョ壒娈婂鐞嗭紝鍏抽敭

	offset++;
	// 椹鹃┒鍛樺鍚嶉暱搴�
	if (offset + 1 > buf_len) {
		return false;
	}
	int namelen = (unsigned char) buf[offset];
	offset++;
	if (offset + namelen > buf_len) {
		return false;
	}
	// 椹鹃┒鍛樺鍚�
	memcpy(dinfo.name, buf + offset, namelen);
	offset += namelen;
	// 浠庝笟璧勬牸璇佺紪鐮�
	if (offset + 20 > buf_len) {
		return false;
	}
	memcpy(dinfo.certification, buf + offset, 20);
	offset += 20;
	// 鍙戣瘉鏈烘瀯鍚嶇О闀垮害
	if (offset + 1 > buf_len) {
		return false;
	}
	int agentlen = (unsigned char) buf[offset];
	offset++;
	if (offset + agentlen > buf_len) {
		return false;
	}
	// 鍙戣瘉鏈烘瀯鍚嶇О
	memcpy(dinfo.agent, buf + offset, agentlen);
	offset += agentlen;
	// 璇佷欢鏈夋晥鏈�
	if (offset + 4 > buf_len) {
		return false;
	}
	memcpy(dinfo.expire, buf + offset, 4);
	return true;
}

/*
 * buf锛氭秷鎭綋
 * buf_len:娑堟伅浣撻暱搴�
 */
string GBHandler::convert_driver_info(char *buf, int buf_len, unsigned char result)
{
	if (NULL == buf || buf_len <= 0)
		return "";

	string sinfo ;
	// 鏂扮増鏈┚椹跺憳韬唤璇嗗埆
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
	} else {  // 鑰佺増鏈�08椹鹃┒鍛樿韩浠借瘑鍒�
		DRIVER_INFO info ;
		if ( ! get_driver_info( buf, buf_len, info ) ) {
			return "" ;
		}
		// 椹鹃┒鍛樺鍚�
		sinfo += ",110:" ;
		sinfo += info.drivername;

		//椹鹃┒鍛樿韩浠借瘉缂栫爜
		sinfo += ",111:" ;
		sinfo += info.driverid;

		//浠庝笟璧勬牸璇佺紪鐮�
		sinfo += ",112:" ;
		sinfo +=info.driverorgid ;

		//鍙戣瘉鏈烘瀯鍚嶇О
		sinfo += ",113:";
		sinfo += info.orgname;
	}
	// 閲嶇粍鏁版嵁杩斿洖
	return "{TYPE:8,RESULT:" + to_string(result) + sinfo+ "}";
}

// 鏌ヨ〃鎵惧�
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

// 灏嗗搴旂殑浣嶈浆鎹㈡垚澶勭悊
static void get_postion_set(const char *buf, string &val)
{
	// 澶勭悊鎵�湁鐘舵�鏍囪
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

// 灏嗗搴旂殑浣嶈浆鎹负鍊�
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

//flag 0:璇诲彇锛�璁剧疆
bool GBHandler::convert_get_para(char *buf, int buf_len, string &data)
{
	/***********寮犻工楂樹慨鏀�-9*************************************/
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
		case 0x0014: //澶囦唤APN
			add_map_key("106", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0015:
			add_map_key("107", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0016:
			add_map_key("108", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0017: //澶囦唤IP
			add_map_key("109", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0018:
			add_map_key("1", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0019: // 鏍规嵁鍐呴儴鍗忚涓庡閮ㄥ崗璁搴斿叧绯诲鐞�
			add_map_key("110", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x001A: //IC鍗¤璇佷富鏈嶅姟鍣ㄥ湴鍧�
			add_map_key("800", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x001B: //IC鍗¤璇佷富鏈嶅姟鍣ㄧ鍙�
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
		case 0x0031: // 鐢靛瓙鍥存爮鍗婂緞
			add_map_key("31", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0040: //鐩戞帶骞冲彴鐢佃瘽鍙风爜
			add_map_key("10", get_buffer(buf + curn, pchar, plen), mp);
			break;
		case 0x0041: //澶嶄綅鐢佃瘽鍙风爜
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
		case 0x0049: //鐩戞帶骞冲彴鐗规潈鐭俊鍙风爜
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
		case 0x005D: // 纰版挒鎶ヨ鍙傛暟璁剧疆
			add_map_key("407", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005E: // 渚х炕鎶ヨ鍙傛暟璁剧疆
			add_map_key("808", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0064: //瀹氭椂鎷嶇収
			add_map_key("809", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0065: //瀹氳窛鎷嶇収
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
		case 0x0084: // 杞︾墝棰滆壊
			add_map_key("42", to_string(buf[curn]), mp);
			break;
		case 0x005B: // 瓒呴�鎶ヨ棰勮宸�
			add_map_key("300", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005C: // 鐗瑰緛绯绘暟
			add_map_key("301", to_string(get_word(buf + curn)), mp);
			break;
		case 0x005F: // 娌圭瀹归噺
			add_map_key("304", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0060: // 浣嶇疆淇℃伅姹囨姤闄勫姞淇℃伅璁剧疆
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
		case 0x0061: // 闂ㄥ紑鍏虫媿鐓ф帶鍒�
		{
			/**
			 add_map_key( "306" , to_string(get_dword(buf+curn)) , mp ) ;
			 */
			string sval;
			get_flagbyword(get_dword(buf + curn), sval, 16);
			add_map_key("306", sval, mp);
		}
			break;
		case 0x0062: // 缁堢澶栧洿浼犳劅閰嶇疆
		{
			/**
			 add_map_key( "307" , to_string(get_dword(buf+curn)) , mp ) ;
			 */
			string sval;
			get_flagbyword(get_dword(buf + curn), sval, 15);
			add_map_key("307", sval, mp);
		}
			break;
		case 0x0063: // 鐩插尯琛ユ姤妯″紡
			add_map_key("308", to_string(get_dword(buf + curn) + 1), mp);
			break;
		case 0x0066: // 閫熷害鏉ユ簮VSS杩樻槸GPS 涓�釜浣嶇殑鏁版嵁
			add_map_key("187", to_string((unsigned char) (*(buf + curn))), mp);
			break;
		case 0x0067: // ToDo: 椹鹃┒鍛樼櫥闄嗘帶鍒朵俊鎭紝瀹囬�鏂板
			add_map_key("190", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0075: // 鍒嗚京鐜�
			add_map_key("309", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0085: // 杞︾墝鍒嗙被
		{
			char szbuf[128] = { 0 };
			add_map_key("310", get_buffer(buf + curn, szbuf, 12), mp);
		}
			break;
		case 0x0090: // GNSS瀹氫綅妯″紡
			add_map_key("811", to_string(buf[curn]), mp);
			break;
		case 0x0091: // GNSS娉㈢壒鐜�
			add_map_key("402", to_string(buf[curn]), mp);
			break;
		case 0x0092: // GNSS妯″潡璇︾粏瀹氫綅鏁版嵁杈撳嚭棰戠巼
			add_map_key("403", to_string(buf[curn]), mp);
			break;
		case 0x0093: // GNSS妯″潡璇︾粏瀹氫綅鏁版嵁閲囬泦棰戠巼锛屽崟浣嶄负绉�
			add_map_key("404", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0094: // GNSS妯″潡璇︾粏瀹氫綅鏁版嵁涓婁紶鏂瑰紡
			add_map_key("815", to_string(buf[curn]), mp);
			break;
		case 0x0095: // GNSS妯″潡璇︾粏瀹氫綅鏁版嵁涓婁紶璁剧疆
			add_map_key("816", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0100: // CAN鎬荤嚎閫氶亾1閲囬泦鏃堕棿闂撮殧(ms)锛�琛ㄧず涓嶉噰闆�
			add_map_key("817", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0101: // CAN鎬荤嚎閫氶亾1涓婁紶鏃堕棿闂撮殧(s)锛�琛ㄧず涓嶄笂浼�
			add_map_key("818", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0102: // CAN鎬荤嚎閫氶亾2閲囬泦鏃堕棿闂撮殧(ms)锛�琛ㄧず涓嶉噰闆�
			add_map_key("819", to_string(get_dword(buf + curn)), mp);
			break;
		case 0x0103: // CAN鎬荤嚎閫氶亾2涓婁紶鏃堕棿闂撮殧(s)锛�琛ㄧず涓嶄笂浼�
			add_map_key("820", to_string(get_word(buf + curn)), mp);
			break;
		case 0x0110:
		{ // CAN鎬荤嚎ID鍗曠嫭閲囬泦璁剧疆
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

// 璁剧疆64鐨勬暟鎹�
static void set_int64(DataBuffer *pbuf, unsigned int msgid, uint64_t n)
{
	pbuf->writeInt32(msgid);
	pbuf->writeInt8(8);
	pbuf->writeInt64(n);
}

// 璁剧疆32浣嶆暟鎹�
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

// 鏇存柊瀵瑰簲浣嶆暟鎹�
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

// 鏋勫缓浣嶇疆淇℃伅璁剧疆鍙傛暟
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

		// 鍙栧�澶勭悊
		nval = atoi(vec[i].c_str());
		npos = (nval - 1) / 2;

		// 鍒ゆ柇鏄惁姝ｇ‘鍊�
		if (npos < 0 || npos > 22) {
			continue;
		}

		nbyte = (flag[npos] - 1) / 8; // 璁＄畻钀藉湪鍝釜鍐呭瓨绌洪棿
		nbit = (flag[npos] - 1) % 8; // 鍙栧緱鏇存柊鍝釜浣�

		if (nval % 2 != 0) {
			// 璁剧疆瀵瑰簲CHAR瀵硅薄浣�
			S_BIT( b[nbyte], nbit);
		}
	}
}

// 灏嗗唴閮ㄥ崗璁殑鍙傛暟璁剧疆杞负澶栭儴鍗忚
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

	//鐢ㄦ潵淇濇寔鍙傛暟鍒楄〃
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
		case 7: //蹇冭烦闂撮殧
			set_dword(pbuf, 0x0001, atoi(v.c_str()));
			pnum++;
			break;
		case 8: //鎶ヨ鍙风爜
		case 10: //姹傚姪鍙风爜
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
		case 9: //鐩戝惉鍙风爜
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
		case 15: //涓績鐭俊鍙风爜
			set_string(pbuf, 0x0043, v.c_str(), v.length());
			pnum++;
			break;
		case 16:
			break;
		case 17:
			break;
		case 18: //璁剧疆GPS鏁版嵁鍥炰紶闂撮殧
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
			set_word(pbuf, 0x0031, atoi(v.c_str())); // 琛ラ〉鏂板鐢靛瓙鍥存爮鍗婂緞
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
		case 41: // 璁剧疆杞︾墝鍙�
			set_string(pbuf, 0x0083, v.c_str(), v.length());
			++pnum;
			break;
		case 42: // 璁剧疆杞︾墝棰滆壊
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
		case 100: //TCP娑堟伅搴旂瓟瓒呮椂鏃堕棿
			set_dword(pbuf, 0x0002, atoi(v.c_str()));
			pnum++;
			break;
		case 101: // TCP閲嶄紶娆℃暟
			set_dword(pbuf, 0x0003, atoi(v.c_str()));
			pnum++;
			break;
		case 102: //UDP
			set_dword(pbuf, 0x0004, atoi(v.c_str()));
			pnum++;
			break;
		case 103: // UDP閲嶄紶娆℃暟
			set_dword(pbuf, 0x0005, atoi(v.c_str()));
			pnum++;
			break;
		case 104: // SMS搴旂瓟瓒呮椂鏃堕棿
			set_dword(pbuf, 0x0006, atoi(v.c_str()));
			pnum++;
			break;
		case 105: // SMS閲嶄紶娆℃暟
			set_dword(pbuf, 0x0007, atoi(v.c_str()));
			pnum++;
			break;
		case 106: //澶囦唤APN
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
		case 109: //澶囦唤鏈嶅姟鍣↖P
			set_string(pbuf, 0x0017, v.c_str(), v.length());
			pnum++;
			break;
		case 110: //鏈嶅姟鍣║DP绔彛
			set_dword(pbuf, 0x0019, atoi(v.c_str()));
			pnum++;
			break;
		case 111: //姹囨姤绛栫暐
			set_dword(pbuf, 0x0020, atoi(v.c_str()));
			pnum++;
			break;
		case 112: // 浣嶇疆姹囨姤
			set_dword(pbuf, 0x0021, atoi(v.c_str()));
			pnum++;
			break;
		case 113:
			set_dword(pbuf, 0x0022, atoi(v.c_str()));
			pnum++;
			break;
		case 114: //浼戠湢鏃朵綅缃眹鎶ユ椂闂撮棿闅�
			set_dword(pbuf, 0x0027, atoi(v.c_str()));
			pnum++;
			break;
		case 115: // 绱ф�鎶ヨ鏃舵眹鎶ユ椂闂撮棿闅�
			set_dword(pbuf, 0x0028, atoi(v.c_str()));
			pnum++;
			break;
		case 116: //
			set_dword(pbuf, 0x0029, atoi(v.c_str()));
			pnum++;
			break;
		case 117: //缂虹渷璺濈姹囨姤闂撮殧锛屽崟浣嶄负绫筹紙m锛夛紝>0
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
		case 122: //澶嶄綅鐢佃瘽鍙风爜锛屽彲閲囩敤姝ょ數璇濆彿鐮佹嫧鎵撶粓绔數璇濊缁堢澶嶄綅
			set_string(pbuf, 0x0041, v.c_str(), v.length());
			pnum++;
			break;
		case 123: //鎭㈠鍑哄巶璁剧疆鐢佃瘽鍙风爜锛屽彲閲囩敤姝ょ數璇濆彿鐮佹嫧鎵撶粓绔數璇濊缁堢鎭㈠鍑哄巶璁剧疆
			set_string(pbuf, 0x0042, v.c_str(), v.length());
			pnum++;
			break;
		case 124: // 鎺ユ敹缁堢SMS鏂囨湰鎶ヨ鍙风爜
			set_string(pbuf, 0x0044, v.c_str(), v.length());
			pnum++;
			break;
		case 125: // 缁堢鐢佃瘽鎺ュ惉绛栫暐
			set_dword(pbuf, 0x0045, atoi(v.c_str()));
			pnum++;
			break;
		case 126: // 姣忔鏈�暱閫氳瘽鏃堕棿
			set_dword(pbuf, 0x0046, atoi(v.c_str()));
			pnum++;
			break;
		case 127: // 褰撴湀鏈�暱閫氳瘽鏃堕棿
			set_dword(pbuf, 0x0047, atoi(v.c_str()));
			pnum++;
			break;
		case 128: //鏈�珮鏃堕�
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
		case 131: //褰撳ぉ绱椹鹃┒鏃堕棿闂ㄩ檺锛屽崟浣嶄负绉掞紙s锛�
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
		case 134: //杞﹁締鎵�湪鐪佸煙ID
			set_word(pbuf, 0x0081, atoi(v.c_str()));
			pnum++;
			break;
		case 135:
			set_word(pbuf, 0x0082, atoi(v.c_str()));
			pnum++;
			break;
		case 136: //鍥惧儚/瑙嗛璐ㄩ噺-1锝�0锛�鏈�ソ;
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
		case 141: //鐩戠骞冲彴鐗规潈鐭俊鍙风爜
			set_string(pbuf, 0x0049, v.c_str(), v.length());
			pnum++;
			break;
		case 142: //鎶ヨ灞忚斀瀛�
			set_dword(pbuf, 0x0050, atoi(v.c_str()));
			pnum++;
			break;
		case 143: //鎶ヨ鍙戦�鏂囨湰寮�叧SMS寮�叧
			set_dword(pbuf, 0x0051, atoi(v.c_str()));
			pnum++;
			break;
		case 144: //鎶ヨ鎷嶆憚寮�叧
			set_dword(pbuf, 0x0052, atoi(v.c_str()));
			pnum++;
			break;
		case 145: //鎶ヨ鎷嶆憚瀛樺偍鏍囧織
			set_dword(pbuf, 0x0053, atoi(v.c_str()));
			pnum++;
			break;
		case 146: //鍏抽敭鎶ヨ鏍囧織
			set_dword(pbuf, 0x0054, atoi(v.c_str()));
			pnum++;
			break;
		case 147: //杞﹁締閲岀▼琛ㄨ鏁�
			set_dword(pbuf, 0x0080, atoi(v.c_str()));
			pnum++;
			break;
		case 180: //瀹氭椂鎷嶇収
			set_dword(pbuf, 0x0064, atoi(v.c_str()));
			pnum++;
			break;
		case 181: //瀹氳窛鎷嶇収
			set_dword(pbuf, 0x0065, atoi(v.c_str()));
			pnum++;
			break;
		case 187: // 璁剧疆VSS閫熷害浼樺厛杩樻槸GPS浼樺厛
			set_word(pbuf, 0x0066, atoi(v.c_str()));
			++pnum;
			break;
		case 190: // ToDo: 璁剧疆椹鹃┒鍛樼櫥闄嗘媿鐓ф帶鍒�瀹囬�鏂板
			set_word(pbuf, 0x0067, atoi(v.c_str()));
			++pnum;
			break;
		case 300: // 瓒呴�鎶ヨ棰勮宸� WORD
			set_word(pbuf, 0x005B, atoi(v.c_str()));
			++pnum;
			break;
		case 301: // 鐤插姵椹鹃┒棰勮宸�
			set_word(pbuf, 0x005C, atoi(v.c_str()));
			++pnum;
			break;
		case 302: // 鐗瑰緛绯绘暟
			set_word(pbuf, 0x005D, atoi(v.c_str()));
			++pnum;
			break;
		case 303: // 杞﹁疆姣忚浆鑴夊啿鏁�
			set_word(pbuf, 0x005E, atoi(v.c_str()));
			++pnum;
			break;
		case 304: // 娌圭瀹归噺
			set_word(pbuf, 0x005F, atoi(v.c_str()));
			++pnum;
			break;
		case 305: // 浣嶇疆淇℃伅姹囨姤闄勫姞璁剧疆
		{
			unsigned char b[32] = { 0 };
			build_postion_set(v, b);
			set_bytes(pbuf, 0x0060, b, 32, 32);
			++pnum;
		}
			break;
		case 306: // 闂ㄥ紑鍏虫媿鐓ф帶鍒�
			set_dword(pbuf, 0x0061, get_dword_by_flag(v));
			++pnum;
			break;
		case 307: // 缁堢澶栧洿浼犳劅閰嶇疆
			set_dword(pbuf, 0x0062, get_dword_by_flag(v));
			++pnum;
			break;
		case 308: // 鐩插尯琛ユ姤妯″紡
			set_dword(pbuf, 0x0063, atoi(v.c_str()) - 1);
			++pnum;
			break;
		case 309: // 鍒嗚京鐜�
			set_word(pbuf, 0x0075, atoi(v.c_str()));
			++pnum;
			break;
		case 310: // 杞︾墝鍒嗙被
			set_bytes(pbuf, 0x0085, (unsigned char*) v.c_str(), v.length(), 12);
			++pnum;
			break;
		case 800: // 閬撹矾杩愯緭璇両C 鍗¤璇佷富鏈嶅姟鍣↖P 鍦板潃鎴栧煙鍚�
			set_string(pbuf, 0x001A, v.c_str(), v.length());
			++pnum;
			break;
		case 801: // 閬撹矾杩愯緭璇両C 鍗¤璇佷富鏈嶅姟鍣═CP 绔彛
			set_dword(pbuf, 0x001B, atoi(v.c_str()));
			++pnum;
			break;
		case 802: // 閬撹矾杩愯緭璇両C 鍗¤璇佷富鏈嶅姟鍣║DP 绔彛
			set_dword(pbuf, 0x001C, atoi(v.c_str()));
			++pnum;
			break;
		case 803: // 閬撹矾杩愯緭璇両C 鍗¤璇佸浠芥湇鍔″櫒IP 鍦板潃鎴栧煙鍚嶏紝绔彛鍚屼富鏈嶅姟鍣�
			set_string(pbuf, 0x001D, v.c_str(), v.length());
			++pnum;
			break;
		/**
		case 805: // 瓒呴�鎶ヨ棰勮宸�锛屽崟浣嶄负1/10Km/h
			set_word(pbuf, 0x005B, atoi(v.c_str()));
			++pnum;
			break;
		case 806: // 鐤插姵椹鹃┒棰勮宸�锛屽崟浣嶄负绉掞紙s锛夛紝>0
			set_word(pbuf, 0x005C, atoi(v.c_str()));
			++pnum;
			break;
		*/
		case 407: // 渚х炕鎶ヨ鍙傛暟璁剧疆锛氫晶缈昏搴︼紝鍗曚綅1 搴︼紝榛樿涓�0 搴�
			set_word(pbuf, 0x005D, atoi(v.c_str()));
			++pnum;
			break;
		case 808: // 纰版挒鎶ヨ鍙傛暟璁剧疆
			set_word(pbuf, 0x005E, atoi(v.c_str()));
			++pnum;
			break;
		case 809: // 瀹氭椂鎷嶇収鎺у埗
			set_dword(pbuf, 0x0064, atoi(v.c_str()));
			++pnum;
			break;
		case 810: // 瀹氳窛鎷嶇収鎺у埗
			set_dword(pbuf, 0x0065, atoi(v.c_str()));
			++pnum;
			break;
		case 811: // GNSS 瀹氫綅妯″紡
			set_word(pbuf, 0x0090, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 402: // GNSS 娉㈢壒鐜�
			set_word(pbuf, 0x0091, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 403: // GNSS 妯″潡璇︾粏瀹氫綅鏁版嵁杈撳嚭棰戠巼
			set_word(pbuf, 0x0092, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 404: // GNSS閲囬泦NMEA鏁版嵁棰戠巼锛屽崟浣嶄负绉�
			set_dword(pbuf, 0x0093, atoi(v.c_str()));
			++pnum;
			break;
		case 815: // GNSS 妯″潡璇︾粏瀹氫綅鏁版嵁涓婁紶鏂瑰紡
			set_word(pbuf, 0x0094, (unsigned char) atoi(v.c_str()));
			++pnum;
			break;
		case 816: // GNSS 妯″潡璇︾粏瀹氫綅鏁版嵁涓婁紶璁剧疆
			set_dword(pbuf, 0x0095, atoi(v.c_str()));
			++pnum;
			break;
		case 817: // CAN 鎬荤嚎閫氶亾1 閲囬泦鏃堕棿闂撮殧(ms)锛� 琛ㄧず涓嶉噰闆�
			set_dword(pbuf, 0x0100, atoi(v.c_str()));
			++pnum;
			break;
		case 818: // CAN 鎬荤嚎閫氶亾1 涓婁紶鏃堕棿闂撮殧(s)锛� 琛ㄧず涓嶄笂浼�
			set_word(pbuf, 0x0101, atoi(v.c_str()));
			++pnum;
			break;
		case 819: // CAN 鎬荤嚎閫氶亾2 閲囬泦鏃堕棿闂撮殧(ms)锛� 琛ㄧず涓嶉噰闆�
			set_dword(pbuf, 0x0102, atoi(v.c_str()));
			++pnum;
			break;
		case 820: // CAN 鎬荤嚎閫氶亾2 涓婁紶鏃堕棿闂撮殧(s)锛� 琛ㄧず涓嶄笂浼�
			set_word(pbuf, 0x0103, atoi(v.c_str()));
			++pnum;
			break;
		case 821: // CAN 鎬荤嚎ID 鍗曠嫭閲囬泦璁剧疆
		{
			// 閲囬泦鏃堕棿闂撮殧|CAN 閫氶亾鍙穦甯х被鍨媩鏁版嵁閲囬泦鏂瑰紡|CAN 鎬荤嚎ID
			unsigned int pick_inter = 0, channel = 0, frametype = 0, pick_way = 0, canid = 0;
			sscanf(v.c_str(), "%u|%u|%u|%u|%u", &pick_inter, &channel, &frametype, &pick_way, &canid);
			// 杞垚64鐨勬暟鎹�
			uint64_t n = 0;
			// 璁剧疆CAN閫氶亾 鍙�
			if (channel > 0)   S_BIT( n, 31 );
			// 琛ㄧず甯х被鍨�
			if (frametype > 0) S_BIT( n, 30 );
			// 琛ㄧず鏁版嵁閲囬泦鏂瑰紡
			if (pick_way > 0)  S_BIT( n, 29 );
			// 琛ㄧずCAN鎬荤嚎ID锛�9浣嶆暟鎹�
			n = ( ( n | (canid & 0x1fff)) & 0x00000000ffffffffLL ) ;
			// 灏嗛珮32浣嶈缃负閲囬泦鏃堕棿闂撮殧
			n |= (((unsigned long long) pick_inter << 32) & 0xffffffff00000000LL);
			// 璁剧疆缁堢鍙傛暟
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

// 娣诲姞鍖楁枟鎷涙爣鐨勬墿灞曟秷鎭笂鎶ュ崗璁�
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
		case 0x0001: // 缁堢灞炴�涓婃姤
		{
			// 缁堢绫诲瀷 | 缁堢鐗堟湰鍙�0x0207,琛ㄧず2.07鐗堟湰) | GNSS妯″潡灞炴� | 閫氫俊妯″潡灞炴� | 缁堢灞炴�
			sprintf(szbuf, ",601:%u|%u|%u|%u|%u",
					get_dword(p + offset), get_dword(p + offset + 4),
					get_dword(p + offset + 8), get_dword(p + offset + 12), get_dword(p + offset + 16));
			data += szbuf;
		}
			break;
		case 0x0002: // 鎵╁睍鏂囨湰淇℃伅涓婂彂鎸囦护
		{
			// 鏍囧織浣嶏綔鏂囨湰淇℃伅(base64)
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

// 缁堢鍗囩骇搴旂瓟
bool GBHandler::convert_term_upgrade_result_notify(const char * buf, int len, char * datap)
{
	if (buf == NULL || len < (int) sizeof(DownUpgradeResult))
		return false;
	// 缁堢鍗囩骇缁撴灉
	DownUpgradeResult *rsp = (DownUpgradeResult *) (buf);
	// 缁勫缓鍐呴儴鍗忚
	sprintf(datap, ",707:%d|%d", rsp->type, rsp->result);

	return true;
}

// 娣诲姞 鏂�08 鏌ヨ缁堢灞炴�搴旂瓟涓婃姤
bool GBHandler::convert_report_new808(const char *buf, int len, char * datap)
{
	int offset = (int) sizeof(TermAttrib);
	if (buf == NULL || len < offset + 2)
		return false;

	// 涓诲姩涓婃姤鐨勭粓绔弬鏁板睘鎬�
	TermAttrib *attr = (TermAttrib *) buf;
	// 缁堢灞炴�绫诲瀷
	unsigned short type = ntohs(attr->type) ;
	// 鍒堕�鍟咺D
	char manuid[6] = { 0 };
	get_buffer((char*) attr->corpid, manuid, sizeof(attr->corpid));
	// 缁堢绫诲瀷
	char term_model[21] = { 0 };
	get_buffer((const char*) attr->termtype, term_model, sizeof(attr->termtype));
	// 缁堢ID
	char term_id[8] = { 0 };
	get_buffer((const char*) attr->termid, term_id, sizeof(attr->termid));
	// 缁堢SIM鍗CCID
	string iccid;
	bcd_to_str((char *) attr->iccid, sizeof(attr->iccid), iccid);

	// 缁堢纭欢鐗堟湰鍙烽暱搴�
	unsigned char nhver = (unsigned char) buf[offset];
	offset = offset + 1;

	// 缁堢纭欢鐗堟湰鍙�
	char hver[257] = { 0 };
	if (nhver > 0)
		get_buffer((const char*) (buf + offset), hver, nhver);
	offset = offset + nhver;

	if (offset > len)
		return false;

	// 缁堢鍥轰欢鐗堟湰鍙烽暱搴�
	unsigned char nfver = (unsigned char) buf[offset];
	offset = offset + 1;

	// 缁堢鍥轰欢鐗堟湰鍙�
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

// 鏋勫缓鍙傛暟鏌ヨ澶勭悊
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
		case 0x0001: // GNSS瀹氫綅妯″紡鍒囨崲
		case 0x0002: // GNSS娉㈢壒鐜囪缃�
		case 0x0003: // GNSS NMEA杈撳嚭鏇存柊鐜囪缃�
		case 0x0004: // GNSS閲囬泦NMEA鏁版嵁棰戠巼
		case 0x0005: // CAN1鍙傛暟璁剧疆
		case 0x0006: // CAN2鍙傛暟璁剧疆
		case 0x0007: // 纰版挒鍙傛暟璁剧疆
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

// 瑙ｆ瀽涓嬪彂鍙傛暟璁剧疆
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

// 瑙ｆ瀽甯︽墿鍙锋爣璇嗙殑鏁版嵁
static bool parse_vector(const string &sval, vector<string> &vec, const char cbegin, const char cend)
{
	size_t end = 0;
	size_t pos = sval.find(cbegin, 0);
	while (pos != string::npos) {
		end = sval.find(cend, pos + 1);
		if (end == string::npos) {
			break;
		}
		// 瀛樻斁瑙ｆ瀽鍑烘潵鐨勬暟鎹甗]鍒嗗壊鏁版嵁
		vec.push_back(sval.substr(pos + 1, end - pos - 1));
		pos = sval.find(cbegin, end + 1);
	}

	if (vec.empty()) {
		return false;
	}

	return true;
}

// 瑙ｆ瀽鎵╁睍鍙傛暟璁剧疆2
bool GBHandler::build_set_param_2ex(vector<DataBuffer*> &vbuf, map<string, string> &mp)
{
	if (mp.empty())
		return false;

	unsigned int flag = 0;
	map<string, string>::iterator it;
	for (it = mp.begin(); it != mp.end(); ++it) {
		flag = atoi(it->first.c_str());
		switch (flag) {
		case 501: // CAN ID鍙傛暟璁剧疆
		{
			// 鎿嶄綔锝淸CAN1璁剧疆椤筣[CAN2璁剧疆椤筣[CAN3璁剧疆椤筣
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
			// ID鍙凤綔ID灞炴�锝淚D鍊硷綔ID椤靛睘鎬э綔ID閲囬泦闂撮殧鏃堕棿
			for (int i = 0; i < size; ++i) {
				vector<string> tmp;
				strsplit(vec[i], "|", tmp);
				if (tmp.size() != 5) {
					continue;
				}
				dbuf.writeInt8(atoi(tmp[0].c_str())); // ID鍙�
				dbuf.writeInt8(atoi(tmp[1].c_str())); // ID灞炴�
				dbuf.writeInt32(atoi(tmp[2].c_str())); // ID鍊�
				dbuf.writeInt8(atoi(tmp[3].c_str())); // ID椤靛睘鎬�
				dbuf.writeInt16(atoi(tmp[4].c_str())); // ID閲囬泦闂撮殧鏃堕棿

				num = num + 1;

				if (dbuf.getDataLen() + 11 > 255) {
					DataBuffer *ptmp = new DataBuffer;
					ptmp->writeInt16(0x0001);
					ptmp->writeInt8(dbuf.getDataLen() + 2);
					ptmp->writeInt8(op); // 璁剧疆鎿嶄綔
					ptmp->writeInt8(num); // 涓暟
					ptmp->writeBytes(dbuf.getData(), dbuf.getDataLen());
					vbuf.push_back(ptmp);
					dbuf.clear();
					num = 0;
				}
			}

			// 澶勭悊鏈�悗涓�釜鏁版嵁
			if (dbuf.getDataLen() > 0) {
				DataBuffer *ptmp = new DataBuffer;
				ptmp->writeInt32(0x0001); // 灏哤ORD鏀规垚DWORD
				ptmp->writeInt8(dbuf.getDataLen() + 2);
				ptmp->writeInt8(op); // 璁剧疆鎿嶄綔
				ptmp->writeInt8(num); // 涓暟
				ptmp->writeBytes(dbuf.getData(), dbuf.getDataLen());
				vbuf.push_back(ptmp);
			}
		}
			break;
		case 502: // 鎵╁睍鏂囨湰涓嬪彂璁剧疆
		{
			// 淇濈暀锛堥粯璁�锛夛綔鏍囧織锝滄枃鏈俊鎭�base64)
			vector<string> vec;
			strsplit(it->second, "|", vec);
			if (vec.size() != 3) {
				continue;
			}
			// 濡傛灉涓嬪彂鏂囨湰涓虹┖
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
			pbuf->writeInt32(0x0002); // 灏哤ORD鏀规垚DWORD
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

// 杞崲鎴愬崄鍏繘鍒�
void GBHandler::print_hex(unsigned char *p, int n, char *buf)
{
	for (int i = 0; i < n; ++i) {
		sprintf(buf + i * 2, "%02x", p[i]);
	}
}

} // namespace triones
