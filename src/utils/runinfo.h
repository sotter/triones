/**
 * author: Triones
 * date  : 2014-08-28
 */
#ifndef __TRIONES_RUNINFO_H__
#define __TRIONES_RUNINFO_H__

#include <time.h>
#include <string>

namespace triones
{

template<class Type>
class SynValue
{
public:
	SynValue(int i = 0)
	:num(i)
	{
	}

	Type value()
	{
		return __sync_fetch_and_add(&num, 0);
	}

	Type reset()
	{
		return __sync_fetch_and_and(&num, 0);
	}

	//前缀++ 和 --, 后缀不是这种方式，使用时注意
	Type operator ++()
	{
		return __sync_add_and_fetch(&num, 1);
	}

	Type operator --()
	{
		return __sync_sub_and_fetch(&num, 1);
	}

	Type operator +=(Type i)
	{
		return __sync_add_and_fetch(&num, i);
	}

	Type operator -=(Type i)
	{
		return __sync_sub_and_fetch(&num, i);
	}

	bool operator ==(Type i)
	{
		return value() == i;
	}

	bool operator >(Type i)
	{
		return value() > i;
	}

	bool operator <(Type i)
	{
		return value() < i;
	}

	bool operator >=(Type i)
	{
		return value() >= i;
	}

	bool operator <=(Type i)
	{
		return value() <= i;
	}
private:
	Type num;
};

class NetRunInfo
{
public:
	NetRunInfo();

	void on_send(int size);

	void on_read(int size);

	std::string host_size(long long size);

	void show();

public:
	struct timeval _btimeval;

	SynValue<long long> _send_total_size;
	SynValue<long long> _recv_total_size;
	SynValue<long long> _send_num;
	SynValue<long long> _recv_num;

	SynValue<long long> _cur_send_total_size;
	SynValue<long long> _cur_recv_total_size;
	SynValue<long long> _cur_send_num;
	SynValue<long long> _cur_recv_num;
};

extern NetRunInfo __net_runinfo;

} // namespace triones

#endif // #ifndef __TRIONES_RUNINFO_H__

