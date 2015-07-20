/**
 * author: Triones
 * date  : 2014-08-28
 */
#include <sys/time.h>
#include <iostream>
#include "../comm/comlog.h"
#include "runinfo.h"

namespace triones
{

NetRunInfo __net_runinfo;

NetRunInfo::NetRunInfo()
{
	gettimeofday(&_btimeval, NULL);
}

void NetRunInfo::on_send(int size)
{
	++_send_num;
	++_cur_send_num;
	_send_total_size += size;
	_cur_send_total_size += size;
}

void NetRunInfo::on_read(int size)
{
	++_recv_num;
	++_cur_recv_num;
	_recv_total_size += size;
	_cur_recv_total_size += size;
}

std::string NetRunInfo::host_size(long long size)
{
	long long mb = size / (1024 * 1024);
	long long kb = (size - (mb * 1024 * 1024)) / 1024;
	long long b = size - (mb * 1024 * 1024) - (kb * 1024);

	char buffer[16] = { 0 };

	if (mb > 0)
	{
		snprintf(buffer, sizeof(buffer) - 1, "%lld.%lldMB", mb, kb);
	}
	else if (kb > 0)
	{
		snprintf(buffer, sizeof(buffer) - 1, "%lld.%lldKB", kb, b);
	}
	else
	{
		snprintf(buffer, sizeof(buffer) - 1, "%lldB", b);
	}

	return buffer;
}

void NetRunInfo::show()
{
	struct timeval cur_timeval;
	gettimeofday(&cur_timeval, NULL);

	//total:%d(updata:%d, online:%d, commret:%d), fail total:%d, current average:%d/s
	//单位是ms
	long long timeval = (cur_timeval.tv_sec - _btimeval.tv_sec) * 1000
	        + ((cur_timeval.tv_usec - _btimeval.tv_usec) / 1000) + 1;

	long long avg_send_num = _cur_send_num.value() * 1000 / timeval;
	long long avg_recv_num = _cur_recv_num.value() * 1000 / timeval;
	long long avg_send_total = _cur_send_total_size.value() * 1000 / timeval;
	long long avg_recv_total = _cur_recv_total_size.value() * 1000 / timeval;

	_cur_send_num.reset();
	_cur_recv_num.reset();
	_cur_send_total_size.reset();
	_cur_recv_total_size.reset();
	gettimeofday(&_btimeval, NULL);

	char buffer[256] = { 0 };

	snprintf(buffer, sizeof(buffer) - 1,
	        "send(avg:%lld/s flux %s/s, total:%lld, send size:%s), recv(avg:%lld/s flux %s/s, total:%lld, recv size:%s)",
	        avg_send_num, host_size(avg_send_total).c_str(), _send_num.value(),
	        host_size(_send_total_size.value()).c_str(), avg_recv_num,
	        host_size(avg_recv_total).c_str(), _recv_num.value(),
	        host_size(_recv_total_size.value()).c_str());

	OUT_INFO(NULL, 0, "net_run_info", buffer);
}

} // namespace triones

