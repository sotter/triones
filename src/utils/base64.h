/**
 * author: Triones
 * date  : 2014-08-26
 */

#ifndef __TRIONES_BASE64_H__
#define __TRIONES_BASE64_H__

namespace triones
{

class Base64
{
public:
	Base64();
	virtual ~Base64();

public:
	bool encode(const char *data, const size_t len);
	bool decode(const char *data, const size_t len);
	char * data();
	size_t size();

private:
	char* _buf;
	size_t _len;
};

} // namespace triones

#endif // #ifndef __TRIONES_BASE64_H__

