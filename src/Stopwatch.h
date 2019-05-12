#ifndef KYSTSOFT_STOPWATCH_H
#define KYSTSOFT_STOPWATCH_H

#include <chrono>

namespace Kystsoft {

class Stopwatch
{
public:
	Stopwatch() { start(); }
	Stopwatch(const Stopwatch& other) = default;
	Stopwatch& operator=(const Stopwatch& rhs) = default;
	void start() { startTime = stopTime = std::chrono::high_resolution_clock::now(); }
	void stop() { stopTime = std::chrono::high_resolution_clock::now(); }
	double seconds() const;
private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point stopTime;
};

} // namespace Kystsoft

#endif // KYSTSOFT_STOPWATCH_H
