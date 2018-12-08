#include "Stopwatch.h"

double Kystsoft::Stopwatch::seconds() const
{
	if (startTime == stopTime)
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime);
		return duration.count();
	}
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(stopTime - startTime);
	return duration.count();
}
