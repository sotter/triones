/*
 * statflux.cpp
 *
 *  Created on: 2015年2月25日
 *      Author: water
 */

#include "statflux.h"

namespace triones
{

CStatFlux::CStatFlux(int span/* = 5*/)
{
	// TODO Auto-generated constructor stub
	_last   = time(0) ;
	_count  = 0;
	_flux   = 0.0f;
	_span   = 5;
	_nstat  = 0;
}

CStatFlux::~CStatFlux()
{
	// TODO Auto-generated destructor stub
}

/*
 * 功能：简单流量统计
 */
void CStatFlux::AddFlux(int n)		// 增加流量
{

	RWGuard guard(_rw_mutex, true);

	// 累加流量
	_count += n;

	time_t now = time(0);

	// 计算某时间段的流量
	if (now - _last >= _span)
	{
		_flux = (float)_count / (float)(now - _last);
		_nstat = _count;
		_last = now;
		_count = 0;
	}
}

/*
 * 功能：取得流量
 */
float CStatFlux::GetFlux(void)
{
	RWGuard guard(_rw_mutex, false);
	return _flux;
}

/*
 * 功能：取得个数
 */
unsigned int CStatFlux::GetCount(void)
{
	RWGuard guard(_rw_mutex, false);
	return _nstat;
}
} /* namespace triones */
