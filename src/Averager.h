#ifndef KYSTSOFT_AVERAGER_H
#define KYSTSOFT_AVERAGER_H

#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

namespace Kystsoft {

template<typename T>
class Averager
{
public:
	Averager(size_t n = 1) : v(n < 1 ? 1 : n) {}
	size_t size() const { return v.size(); }
	void resize(size_t n) { v.resize(n < 1 ? 1 : n); }
	void add(T value);
	Averager& operator+=(T value) { add(value); return *this; }
	T average() const { return std::accumulate(v.begin(), v.end(), T()) / v.size(); }
	operator T() const { return average(); }
	void setSamplingInterval(double interval);
	void setAveragingInterval(double interval);
private:
	std::vector<T> v;
	size_t i = 0; // next index
	double samplingInterval = 1;
};

} // namespace Kystsoft

template<typename T>
void Kystsoft::Averager<T>::add(T value)
{
	v[i] = value;
	if (++i >= v.size())
		i = 0;
}

template<typename T>
void Kystsoft::Averager<T>::setSamplingInterval(double interval)
{
	samplingInterval = std::fabs(interval);
	if (samplingInterval == 0)
		samplingInterval = 1;
}

template<typename T>
void Kystsoft::Averager<T>::setAveragingInterval(double interval)
{
	resize(size_t(std::fabs(interval) / samplingInterval + 0.5));
}

#endif // KYSTSOFT_AVERAGER_H
