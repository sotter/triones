/**
 * author: Triones
 * date  : 2014-08-21
 */

#ifndef __TRIONES_ATOMIC_H__
#define __TRIONES_ATOMIC_H__

namespace triones
{

class Atomic
{
public:
	Atomic()
			: _value(0)
	{
	}

	operator int()
	{
		return _value;
	}

	void atomic_inc()
	{
		__sync_add_and_fetch(&_value, 1);
	}

	void atomic_dec()
	{
		__sync_sub_and_fetch(&_value, 1);
	}

	void atomic_add(int i)
	{
		__sync_add_and_fetch(&_value, i);
	}

	void atomic_sub(int i)
	{
		__sync_sub_and_fetch(&_value, i);
	}

	int atomic_fetch()
	{
		return __sync_add_and_fetch(&_value, 0);
	}

	Atomic& operator ++()
	{
		__sync_add_and_fetch(&_value, 1);
		return *this;
	}

	Atomic& operator --()
	{
		__sync_sub_and_fetch(&_value, 1);
		return *this;
	}

	Atomic& operator +=(int i)
	{
		__sync_add_and_fetch(&_value, i);
		return *this;
	}

	Atomic& operator -=(int i)
	{
		__sync_sub_and_fetch(&_value, i);
		return *this;
	}

	bool operator ==(int i)
	{
		return __sync_add_and_fetch(&_value, 0) == i;
	}

	bool operator >(int i)
	{
		return __sync_add_and_fetch(&_value, 0) > i;
	}

	bool operator <(int i)
	{
		return __sync_add_and_fetch(&_value, 0) < i;
	}

	bool operator >=(int i)
	{
		return __sync_add_and_fetch(&_value, 0) >= i;
	}

	bool operator <=(int i)
	{
		return __sync_add_and_fetch(&_value, 0) <= i;
	}
private:
	int _value;
};

#define ATOMIC_DEC(x)   ((x)->atomic_dec())
#define ATOMIC_ADD(x,y) ((x)->atomic_add(y))
#define ATOMIC_SUB(x,y) ((x)->atomic_sub(y))
#define ATOMIC_FETCH(x) ((x)->atomic_fetch(x))

} // namespace triones

#endif // #ifndef __TRIONES_ATOMIC_H__
