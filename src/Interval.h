#ifndef KYSTSOFT_INTERVAL_H
#define KYSTSOFT_INTERVAL_H

#include <algorithm>
#include <istream>
#include <limits>
#include <sstream>
#include <string>

namespace Kystsoft {

// Interval is lower inclusive and upper exclusive
template<typename T>
class Interval
{
public:
	Interval() {}
	Interval(T lower, T upper) : low(lower), upp(upper) {}
	bool load(const std::string& str) { std::istringstream is(str); return load(is); }
	bool load(std::istream& is);
	bool isValid() const { return low < upp; }
	bool isFinite() const { T inf = std::numeric_limits<T>::infinity(); return -inf < low && upp < inf; }
	bool includes(T x) const { return low <= x && x < upp; }
	T width() const { return upp - low; }
	T lower() const { return low; }
	T upper() const { return upp; }
	void setLower(T lower) { low = lower; }
	void setUpper(T upper) { upp = upper; }
	void set(T lower, T upper) { low = lower; upp = upper; }
	void clearLower() { low = -std::numeric_limits<T>::infinity(); }
	void clearUpper() { upp = std::numeric_limits<T>::infinity(); }
	void clear() { clearLower(); clearUpper(); }
private:
	T low = -std::numeric_limits<T>::infinity();
	T upp = std::numeric_limits<T>::infinity();
};

} // namespace Kystsoft

template<typename T>
bool Kystsoft::Interval<T>::load(std::istream& is)
{
	std::string s1, s2, s3;
	if (!(is >> s1 >> s2 >> s3))
		return false;
	std::istringstream is1(s1);
	std::istringstream is3(s3);
	T lower = -std::numeric_limits<T>::infinity();
	T upper = std::numeric_limits<T>::infinity();
	is1 >> lower;
	is3 >> upper;
	low = std::min(lower, upper);
	upp = std::max(lower, upper);
	return true;
}

#endif // KYSTSOFT_INTERVAL_H
