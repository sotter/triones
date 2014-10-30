/**
 * author: Triones
 * date  : 2014-08-21
 */

#ifndef __TRIONES_EXCEPTION_H__
#define __TRIONES_EXCEPTION_H__

#include <string>
#include <exception>

namespace triones
{

class TException: public std::exception
{
public:
	TException()
	{
	}

	TException(const std::string& message) :
			_message(message)
	{
	}

	virtual ~TException() throw ()
	{
	}

	virtual const char* what() const throw ()
	{
		if (_message.empty())
		{
			return "Default TException.";
		}
		else
		{
			return _message.c_str();
		}
	}

protected:
	std::string _message;

};

class NoSuchTaskException: public TException
{
};

class UncancellableTaskException: public TException
{
};

class InvalidArgumentException: public TException
{
};

class IllegalStateException: public TException
{
};

class TimedOutException: public TException
{
public:
	TimedOutException() :
			TException("TimedOutException")
	{
	}
	;
	TimedOutException(const std::string& message) :
			TException(message)
	{
	}
};

class TooManyPendingTasksException: public TException
{
public:
	TooManyPendingTasksException() :
			TException("TooManyPendingTasksException")
	{
	}
	;
	TooManyPendingTasksException(const std::string& message) :
			TException(message)
	{
	}
};

class SystemResourceException: public TException
{
public:
	SystemResourceException()
	{
	}

	SystemResourceException(const std::string& message) :
			TException(message)
	{
	}
};


} // namespace triones

#endif // #ifndef __TRIONES_EXCEPTION_H__

