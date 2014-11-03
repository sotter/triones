/**
 * author: Triones
 * date  : 2014-08-26
 */
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>
#include <iconv.h>
#include <limits.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "str.h"

namespace triones
{

// trim from start
std::string& ltrim(std::string& s)
{
	s.erase(s.begin(),
	        std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string& rtrim(std::string& s)
{
	s.erase(
	        std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
	        s.end());
	return s;
}

// trim from both ends
std::string& trim(std::string& s)
{
	return ltrim(rtrim(s));
}

std::string& tolower(std::string& str)
{
	std::string::iterator first = str.begin(), last = str.end(), result = str.begin();
	while (first != last)
		*result++ = std::tolower(*first++);
	return str;
}

std::string& toupper(std::string& str)
{
	std::string::iterator first = str.begin(), last = str.end(), result = str.begin();
	while (first != last)
		*result++ = std::toupper(*first++);
	return str;
}

// compare strings case-insensitively.
int stricmp(const std::string& ls, const std::string& rs)
{
	std::string::const_iterator lit = ls.begin();
	std::string::const_iterator rit = rs.begin();

	for (; lit != ls.end() && rit != rs.end(); ++lit, ++rit)
	{
		if (std::toupper(*lit) == std::toupper(*rit))
		{
			continue;
		}

		if (std::toupper(*lit) > std::toupper(*rit))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	if (lit == ls.end() && rit == rs.end())
	{
		return 0;
	}
	else if (lit == ls.end())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

// The second parameter is a set of separators, i.e. each char is one separator.
// e.g. if the separators is "sep", string "theseptember" will be split into "th", "","","","t","mb" and "r".
void strsplit(const std::string& str, const std::string& separators,
        std::vector<std::string> & str_list, bool skip /* = false*/)
{
	str_list.clear();

	size_t head = 0;
	size_t tail = str.find_first_of(separators, head);

	for (; tail != str.npos; head = tail + 1, tail = str.find_first_of(separators, head))
	{
		if (tail == head && skip)
		{
			continue;
		}
		str_list.push_back(str.substr(head, tail - head));
	}

	if ((!skip && head > 0) || str.substr(head).size() > 0)
	{
		str_list.push_back(str.substr(head));
	}

	return;
}

// The second parameter is THE separator.
// e.g. if the separator is "sep", string "theseptember" will be split into "the" and "tember".
void strsplit2(const std::string& str, const std::string& separator,
        std::vector<std::string> & str_list, bool skip /* = false*/)
{
	str_list.clear();

	size_t head = 0;
	size_t tail = str.find(separator, head);

	for (; tail != str.npos; head = tail + separator.size(), tail = str.find(separator, head))
	{
		if (tail == head && skip)
		{
			continue;
		}
		str_list.push_back(str.substr(head, tail - head));
	}

	if ((!skip && head > 0) || str.substr(head).size() > 0)
	{
		str_list.push_back(str.substr(head));
	}

	return;
}

std::string& replace(std::string& str, const std::string& str1, const std::string& str2)
{
	size_t slen = str1.size();
	size_t dlen = str2.size();
	for (size_t pos = str.find(str1); pos != std::string::npos;)
	{
		str.replace(pos, slen, str2);
		pos += dlen;
		pos = str.find(str1, pos);
	}

	return str;
}

bool strformat(std::string& str, const char* fmt, ...)
{
	int size = 32;
	va_list ap;

	while (1)
	{
		str.resize(size);

		va_start(ap, fmt);
		int n = vsnprintf((char *) str.data(), size, fmt, ap);
		va_end(ap);

		if (n > -1 && n < size)
		{
			str.resize(n);
			return true;
		}

		if (n > -1)
		{
			size = n + 1;
		}
		else
		{
			str.clear();
			return false;
		}
	}

	return true;
}

std::string to_string(int8_t val)
{
	std::string str;

	if (!strformat(str, "%" PRId8, val)) str.clear();

	return str;
}

std::string to_string(uint8_t val)
{
	std::string str;

	if (!strformat(str, "%" PRIu8, val)) str.clear();

	return str;
}

std::string to_string(int16_t val)
{
	std::string str;

	if (!strformat(str, "%" PRId16, val)) str.clear();

	return str;
}

std::string to_string(uint16_t val)
{
	std::string str;

	if (!strformat(str, "%" PRIu16, val)) str.clear();

	return str;
}

std::string to_string(int32_t val)
{
	std::string str;

	if (!strformat(str, "%" PRId32, val)) str.clear();

	return str;
}

std::string to_string(uint32_t val)
{
	std::string str;

	if (!strformat(str, "%" PRIu32, val)) str.clear();

	return str;
}

std::string to_string(int64_t val)
{
	std::string str;

	if (!strformat(str, "%" PRId64, val)) str.clear();

	return str;
}

std::string to_string(uint64_t val)
{
	std::string str;

	if (!strformat(str, "%" PRIu64, val)) str.clear();

	return str;
}

std::string to_string(float val)
{
	std::string str;

	if (!strformat(str, "%f", val)) str.clear();

	return str;
}

std::string to_string(double val)
{
	std::string str;

	if (!strformat(str, "%f", val)) str.clear();

	return str;
}

std::string to_string(long double val)
{
	std::string str;

	if (!strformat(str, "%Lf", val)) str.clear();

	return str;
}

bool bin_to_hex(const char* bin, size_t len, std::string& hex)
{
	hex.clear();

	for (size_t i = 0; i < len; ++i)
	{
		char ch = bin[i];

		// 高4位
		ch >>= 4;
		ch &= 0x0f;
		ch += (ch <= 9) ? '0' : ('a' - 10);

		hex.append(1, ch);

		// 低4位
		ch = bin[i];
		ch &= 0x0f;
		ch += (ch <= 9) ? '0' : ('a' - 10);

		hex.append(1, ch);
	}
	return true;
}

bool hex_to_bin(const char* hex, size_t len, std::string& bin)
{
	if (len % 2 != 0)
	{
		return false;
	}

	bin.clear();

	for (size_t i = 0; i < len / 2; i++)
	{
		char ch[2];

		for (int n = 0; n <= 1; ++n)
		{
			ch[n] = hex[2 * i + n];
			if (ch[n] >= '0' && ch[n] <= '9')
			{
				ch[n] = ch[n] - '0';
			}
			else if (ch[n] >= 'A' && ch[n] <= 'F')
			{
				ch[n] = ch[n] - 'A' + 10;
			}
			else if (ch[n] >= 'a' && ch[n] <= 'f')
			{
				ch[n] = ch[n] - 'a' + 10;
			}
			else
			{
				bin.clear();
				return false;
			}
		}

		char chr = ((ch[0] << 4) & 0xf0) | (ch[1] & 0x0f);
		bin.append(1, chr);
	}

	return true;
}

bool str_to_bcd(const char* str, size_t len, std::string& bcd)
{
	bcd.clear();

	for (size_t i = 0; i < len; i += 2)
	{
		char ch[2];

		ch[0] = str[i] - '0';

		if (ch[0] < 0 || ch[0] > 9)
		{
			bcd.clear();
			return false;
		}

		if (i + 1 < len)
		{
			ch[1] = str[i + 1] - '0';

			if (ch[1] < 0 || ch[1] > 9)
			{
				bcd.clear();
				return false;
			}
		}
		else // 末尾补齐
		{
			ch[1] = 0xff;
		}

		char chr = ((ch[0] << 4) & 0xf0) | (ch[1] & 0x0f);
		bcd.append(1, chr);
	}

	return true;
}

bool bcd_to_str(const char* bcd, size_t len, std::string& str)
{
	str.clear();

	for (size_t i = 0; i < len; ++i)
	{
		char ch = bcd[i];

		// 高4位
		ch >>= 4;
		ch &= 0x0f;

		if (ch < 0 || ch > 9)
		{
			str.clear();
			return false;
		}

		ch += '0';

		str.append(1, ch);

		// 低4位
		ch = bcd[i];
		ch &= 0x0f;

		// 如果是末尾补齐，直接返回
		if (ch == 0x0f && i + 1 == len)
		{
			return true;
		}

		if (ch < 0 || ch > 9)
		{
			str.clear();
			return false;
		}

		ch += '0';

		str.append(1, ch);
	}
	return true;
}

bool int_to_bcd(unsigned int ui, std::string& bcd)
{
	std::string istr;

	do
	{
		char c = (ui % 10) + '0';
		istr.insert(0, 1, c);
		ui /= 10;
	} while (ui > 0);

	return str_to_bcd(istr.data(), istr.size(), bcd);
}

bool bcd_to_int(const char* bcd, size_t len, unsigned int& ui)
{
	std::string istr;

	if (!bcd_to_str(bcd, len, istr))
	{
		return false;
	}

	ui = 0;

	for (size_t i = 0; i < istr.size(); ++i)
	{
		char c = istr[i] - '0';
		ui = ui * 10 + c;

		if (ui >= UINT_MAX)
		{
			return false;
		}
	}

	return true;
}

// for from_code and to_code see `iconv --list`
bool strconv(std::string& content, const char* from_code, const char* to_code)
{
	iconv_t cd;
	if ((cd = iconv_open(to_code, from_code)) == (iconv_t) -1)
	{
		content = "";
		return false;
	}

	iconv(cd, NULL, NULL, NULL, NULL);

	size_t inl = content.length();
	size_t oul = 4 * inl + 1024;

	char *in = (char *) content.data();
	char *ous = new char[oul];
	char *oun = ous;

	int ret = iconv(cd, &in, (size_t *) &inl, &oun, (size_t *) &oul);
	if (ret != -1)
	{
		content.assign(ous, oun - ous);
	}
	else
	{
		content = "";
	}

	delete[] ous;
	iconv_close(cd);

	if (ret == -1)
	{
		return false;
	}
	return true;
}

} // namespace triones

