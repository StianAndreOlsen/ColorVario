#ifndef STD_ALGORITHM_H
#define STD_ALGORITHM_H

// Algorithms that will be part of C++17
// TODO: Remove when C++17 is available

namespace std {

template<typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

} // namespace std

#endif // STD_ALGORITHM_H
