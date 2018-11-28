#include "ValueColor.h"

void Kystsoft::ValueColor::load(const Settings& settings, const std::string& section)
{
	// color on and off zones
	zones.load(settings, section);

	// color points
	clearColorPoints();
	for (const auto& value : settings.values(section + ".point"))
	{
		ValueColorPoint point;
		if (point.load(value))
			addColorPoint(point);
	}
}

Kystsoft::Color Kystsoft::ValueColor::color(double value) const
{
	if (zones.isOff(value))
		return Dali::Color::TRANSPARENT;
	if (colorPoints.empty())
		return Color::error();
	if (colorPoints.size() < 2)
		return colorPoints[0].color;
	if (value <= colorPoints.front().value)
		return colorPoints.front().color;
	if (value >= colorPoints.back().value)
		return colorPoints.back().color;
	size_t i = colorPointInterval(value);
	double x = value;
	double x1 = colorPoints[i].value;
	double x2 = colorPoints[i+1].value;
	Color c1 = colorPoints[i].color;
	Color c2 = colorPoints[i+1].color;
	return c1.blended(c2, float((x - x1) / (x2 - x1)));
}

size_t Kystsoft::ValueColor::colorPointInterval(double value) const
{
	if (colorPoints.size() > 2)
		for (size_t i = colorPoints.size() - 2; i > 0; --i)
			if (value >= colorPoints[i].value)
				return i;
	return 0;
}
