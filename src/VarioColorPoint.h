#ifndef KYSTSOFT_VARIOCOLORPOINT_H
#define KYSTSOFT_VARIOCOLORPOINT_H

#include "Color.h"
#include <iostream>

namespace Kystsoft {

struct VarioColorPoint
{
	float climb = 0; // vertical speed [m/s]
	Color color;
	VarioColorPoint(float clmb, const Color& clr) : climb(clmb), color(clr) {}
	bool load(std::istream& is, char delimiter = '\t');
	void save(std::ostream& os, char delimiter = '\t') const;
	bool operator<(const VarioColorPoint& other) const
		{ return climb < other.climb; }
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCOLORPOINT_H
