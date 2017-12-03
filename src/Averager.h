#ifndef KYSTSOFT_AVERAGER_H
#define KYSTSOFT_AVERAGER_H

#include <cstddef>
#include <numeric>
#include <vector>

namespace Kystsoft {

class Averager
{
public:
	Averager(size_t n) : v(n, 0) {}
	void resize(size_t n) { v.resize(n, 0); }
	void add(float value);
	Averager& operator+=(float value) { add(value); return *this; }
	float average() const { return std::accumulate(v.begin(), v.end(), 0.0f) / v.size(); }
	operator float() const { return average(); }
private:
	std::vector<float> v;
	size_t i = 0; // next index
};

} // namespace Kystsoft

#endif // KYSTSOFT_AVERAGER_H
