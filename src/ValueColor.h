#ifndef KYSTSOFT_VALUECOLOR_H
#define KYSTSOFT_VALUECOLOR_H

#include "Settings.h"
#include "ValueColorPoint.h"

namespace Kystsoft {

class ValueColor
{
public:
	void load(const Settings& settings, const std::string& section);
	void addColorPoint(const ValueColorPoint& point) { colorPoints.push_back(point); }
	void addColorPoint(double value, const Color& color)
		{ addColorPoint(ValueColorPoint(value, color)); }
	void clearColorPoints() { colorPoints.clear(); }
	Color color(double value) const;
	Color operator()(double value) const { return color(value); }
private:
	size_t colorPointInterval(double value) const;
	std::vector<ValueColorPoint> colorPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUECOLOR_H
