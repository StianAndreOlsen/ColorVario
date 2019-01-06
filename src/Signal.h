#ifndef KYSTSOFT_SIGNAL_H
#define KYSTSOFT_SIGNAL_H

#include <functional>
#include <map>

// This Signal class is a slightly modified version of the
// Signal class found in Simon Schneegans blog post:
// http://simmesimme.github.io/tutorials/2015/09/20/signal-slot
// Most members are declared const and can consequently be
// called through a const reference. Notably, emit and
// disconnectAll is not const and cannot be accessed through a
// const reference.

// A signal object may call multiple slots with the
// same signature. You can connect functions to the signal
// which will be called when the emit() method on the
// signal object is invoked. Any argument passed to emit()
// will be passed to the connected functions.

namespace Kystsoft {

template<typename... Args>
class Signal
{
public:
	Signal() {}
	Signal(const Signal& other) = delete;
	Signal& operator=(const Signal& rhs) = delete;

	// connection identifier that can be used to disconnect the function again
	using ConnectionId = size_t;

	// connect a std::function
	ConnectionId connect(const std::function<void(Args...)>& slot) const
	{
		slots.insert(std::make_pair(nextId, slot));
		return nextId++;
	}

	// connect a member function
	template<typename T>
	ConnectionId connect(T* instance, void (T::*function)(Args...)) const
	{
		return connect([=](Args... args)
		{
			(instance->*function)(args...);
		});
	}

	// connect a const member function
	template<typename T>
	ConnectionId connect(T* instance, void (T::*function)(Args...) const) const
	{
		return connect([=](Args... args)
		{
			(instance->*function)(args...);
		});
	}

	// disconnect a previously connected function
	void disconnect(ConnectionId id) const
	{
		slots.erase(id);
	}

	// disconnect all previously connected functions
	void disconnectAll()
	{
		slots.clear();
	}

	// call all connected functions
	void emit(Args... p)
	{
		for (auto it : slots)
			it.second(p...);
	}

private:
	mutable std::map<ConnectionId, std::function<void(Args...)>> slots;
	mutable ConnectionId nextId = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SIGNAL_H
