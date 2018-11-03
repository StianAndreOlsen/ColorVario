#ifndef KYSTSOFT_VALUECOLORPOINT_H
#define KYSTSOFT_VALUECOLORPOINT_H

#include "Color.h"
#include <istream>
#include <sstream>
#include <string>

namespace Kystsoft {

struct ValueColorPoint
{
	double value = 0; // controlling value
	Color color;
	ValueColorPoint() {}
	ValueColorPoint(double v, const Color& c) : value(v), color(c) {}
	bool load(const std::string& str) { std::istringstream is(str); return load(is); }
	bool load(std::istream& is);
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUECOLORPOINT_H
