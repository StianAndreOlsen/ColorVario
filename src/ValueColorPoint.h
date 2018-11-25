#ifndef KYSTSOFT_VALUECOLORPOINT_H
#define KYSTSOFT_VALUECOLORPOINT_H

#include "Color.h"

namespace Kystsoft {

struct ValueColorPoint
{
	double value = 0; // controlling value
	Color color;
	ValueColorPoint() {}
	ValueColorPoint(double v, const Color& c) : value(v), color(c) {}
	bool load(const std::string& str);
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUECOLORPOINT_H
