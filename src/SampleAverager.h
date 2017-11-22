#ifndef KYSTSOFT_SAMPLEAVERAGER_H
#define KYSTSOFT_SAMPLEAVERAGER_H

#include <cstddef>
#include <numeric>
#include <vector>

namespace Kystsoft {

class SampleAverager
{
public:
	SampleAverager(size_t n) : v(n, 0) {}
	void resize(size_t n) { v.resize(n, 0); }
	void add(float value);
	SampleAverager& operator+=(float value) { add(value); return *this; }
	float average() const { return std::accumulate(v.begin(), v.end(), 0.0f) / v.size(); }
	operator float() const { return average(); }
private:
	size_t i = 0; // next index
	std::vector<float> v;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SAMPLEAVERAGER_H
