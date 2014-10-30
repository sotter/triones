/**
 * author: Triones
 * date  : 2014-08-21
 * desc  : 引用计数对象，当引用值为零时释放对象
 */

#ifndef __TRIONES_REF_H__
#define __TRIONES_REF_H__

namespace triones
{

class Ref
{
public:
	Ref();
	virtual ~Ref()
	{
	}
	;

	// 添加引用
	int add_ref();

	// 取得引用
	int get_ref();

	// 释放引用
	void release();

private:
	// 引用记数值
	unsigned int _ref;
};

} // namespace triones

#endif // #ifndef __TRIONES_REF_H__
