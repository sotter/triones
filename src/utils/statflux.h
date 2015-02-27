/*
 * statflux.h
 *
 *  Created on: 2015年2月25日
 *      Author: water
 */

#ifndef STATFLUX_H_
#define STATFLUX_H_

namespace triones
{

class CStatFlux
{
public:
	CStatFlux(int span = 5);
	virtual ~CStatFlux();

	/*
	 * 功能：简单流量统计
	 */
	void AddFlux(int n);		// 增加流量

	/*
	 * 功能：取得流量
	 */
	float GetFlux(void);

	/*
	 * 功能：取得个数
	 */
	unsigned int GetCount(void);
private:
	// 同步锁
	RWMutex _rw_mutex;
	// 最后一次时间
	time_t _last;
	// 数据量统计
	unsigned int _count;
	// 最后一次流量
	float _flux;
	// 平均时间间隔
	unsigned int _span;
	// 最后条数记数
	unsigned int _nstat;
};

} /* namespace triones */

#endif /* STATFLUX_H_ */
