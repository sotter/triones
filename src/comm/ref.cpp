/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 引用计数对象，当引用值为零时释放对象
 *
 * update: 2012.12.10
 *         修改引用计数据对象，将锁使用全局锁，因为它的作用时间很短，整个程序可以共用一个锁
 *         这样可以减少应用中锁的资源占用
 */

#include <assert.h>

#include "ref.h"
#include "mutex.h"

namespace triones
{

/**
 * 引用记数锁
 */
static Mutex g_mutex;

Ref::Ref()
		: _ref(0)
{
}

/**
 * 添加引用
 */
int Ref::add_ref()
{
	g_mutex.lock();
	++_ref;
	g_mutex.unlock();

	return _ref;
}

/**
 * 取得引用
 */
int Ref::get_ref()
{
	Guard g(g_mutex);

	return _ref;
}

/**
 * 释放引用
 */
void Ref::release()
{
	bool destory = false;
	{
		g_mutex.lock();

		assert(_ref > 0);
		--_ref;

		if (_ref == 0)
		{
			destory = true;
		}
		g_mutex.unlock();
	}
	if (destory)
	{
		delete this;
	}
}

} // namespace triones
