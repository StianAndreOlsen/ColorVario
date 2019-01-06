#ifndef KYSTSOFT_CALLBACK_H
#define KYSTSOFT_CALLBACK_H

#include <functional>

// The Callback class is modeled directly from the Signal
// class. The difference is that a callback can only be
// connected to one function while a signal can be connected
// to multiple slots/functions. Furthermore, even the execute
// function is declared const and can be accessed through a
// const reference.
//
// The connected function is called when the execute() method
// on the callback object is invoked. Any arguments passed to
// execute() will be passed to the connected function.

namespace Kystsoft {

template<typename... Args>
class Callback
{
public:
	Callback() {}
	Callback(const Callback& other) = delete;
	Callback& operator=(const Callback& rhs) = delete;

	// connect a std::function
	void connect(const std::function<void(Args...)>& function) const
	{
		func = function;
	}

	// connect a member function
	template<typename T>
	void connect(T* instance, void (T::*function)(Args...)) const
	{
		connect([=](Args... args)
		{
			(instance->*function)(args...);
		});
	}

	// connect a const member function
	template<typename T>
	void connect(T* instance, void (T::*function)(Args...) const) const
	{
		connect([=](Args... args)
		{
			(instance->*function)(args...);
		});
	}

	// disconnect a previously connected function
	void disconnect() const
	{
		func = std::function<void(Args...)>();
	}

	// call connected function
	void execute(Args... p) const
	{
		if (func)
			func(p...);
	}

private:
	mutable std::function<void(Args...)> func;
};

} // namespace Kystsoft

#endif // KYSTSOFT_CALLBACK_H
