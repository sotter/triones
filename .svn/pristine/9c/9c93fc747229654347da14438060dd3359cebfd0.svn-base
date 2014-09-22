/**
 * author: Triones
 * date  : 2014-08-26
 */

#ifndef __TRIONES_STRING_H__
#define __TRIONES_STRING_H__

#include <string>
#include <vector>
#include <stdint.h>

namespace triones
{

//
// ltrim:
//   去除字串 s 左边的空白字符。返回修改后的 s 的引用。空白字符包括空格\r\n\t等。
//
// rtrim:
//   去除字串 s 右边的空白字符。返回修改后的 s 的引用。空白字符包括空格\r\n\t等。
//
// trim:
//   去除字串 s 两侧的空白字符。返回修改后的 s 的引用。空白字符包括空格\r\n\t等。
//
std::string& ltrim(std::string& s);
std::string& rtrim(std::string& s);
std::string& trim(std::string & s);

//
// tolower:
//   把字符串转换为小写。返回修改后端 str 的引用。
//
// toupper:
//   把字符串转换为大写。返回修改后端 str 的引用。
//
std::string& tolower(std::string& str);
std::string& toupper(std::string& str);

// compare strings case-insensitively.
int stricmp(const std::string& ls, const std::string& rs);

// The second parameter is a set of separators, i.e. each char is one separator.
// If the separators is "sep", string "theseptember" will be split into "th", "","","","t","mb" and "r".
void strsplit(const std::string& str, const std::string& separators, std::vector<std::string> & str_list, bool skip = false);

// The second parameter is THE separator.
// If the separator is "sep", string "theseptember" will be split into "the" and "tember".
void strsplit2(const std::string& str, const std::string& separator, std::vector<std::string> & str_list, bool skip = false);

//
// replace:
//   把 str 里的所有 str1 替换成 str2。返回修改后的 str 的引用。
//
std::string& replace(std::string& str, const std::string& str1, const std::string& str2);

//
// strformat:
// 	类似 vsnprintf，只不过是把生成的字符串放到 str 中。
//
bool strformat(std::string& str, const char* fmt, ...);

std::string to_string(int8_t val);
std::string to_string(uint8_t val);
std::string to_string(int16_t val);
std::string to_string(uint16_t val);
std::string to_string(int32_t val);
std::string to_string(uint32_t val);
std::string to_string(int64_t val);
std::string to_string(uint64_t val);
std::string to_string(float val);
std::string to_string(double val);
std::string to_string(long double val);

bool bin_to_hex(const char* bin, size_t len, std::string& hex);
bool hex_to_bin(const char* hex, size_t len, std::string& bin);
bool str_to_bcd(const char* str, size_t len, std::string& bcd);
bool bcd_to_str(const char* bcd, size_t len, std::string& str);
bool int_to_bcd(unsigned int ui, std::string& bcd);
bool bcd_to_int(const char* bcd, size_t len, unsigned int& ui);

//
// strconv:
//   convert string from one codeset to another.
//   for from_code and to_code see `iconv --list`.
//
bool strconv(std::string& content, const char* from_code, const char* to_code);

} // namespace triones

#endif // #ifndef __TRIONES_STRING_H__
