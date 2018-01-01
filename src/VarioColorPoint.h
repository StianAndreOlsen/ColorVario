#ifndef KYSTSOFT_VARIOCOLORPOINT_H
#define KYSTSOFT_VARIOCOLORPOINT_H

#include "Color.h"
#include <istream>

namespace Kystsoft {

struct VarioColorPoint
{
	float climb = 0; // vertical speed [m/s]
	Color color;
	VarioColorPoint() {}
	VarioColorPoint(float clmb, const Color& clr) : climb(clmb), color(clr) {}
	bool load(std::istream& is);
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCOLORPOINT_H
