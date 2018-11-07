#ifndef KYSTSOFT_INTERVALS_H
#define KYSTSOFT_INTERVALS_H

#include "Interval.h"
#include <vector>

namespace Kystsoft {

template<typename T>
class Intervals : public std::vector<Interval<T>>
{
public:
	bool include(T x) const
	{
		for (const auto& interval : *this)
			if (interval.includes(x))
				return true;
		return false;
	}
};

} // namespace Kystsoft

#endif // KYSTSOFT_INTERVALS_H
