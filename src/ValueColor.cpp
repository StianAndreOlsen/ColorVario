#include "ValueColor.h"

void Kystsoft::ValueColor::load(const Settings& settings, const std::string& section)
{
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
	if (colorPoints.empty())
		return Dali::Color::RED;
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
	double y1 = colorPoints[i].color.hue();
	double y2 = colorPoints[i+1].color.hue();
	if (y1 == Color::hueUndefined || y2 == Color::hueUndefined)
	{
		// interpolate RGBA values
		y1 = colorPoints[i].color.red();
		y2 = colorPoints[i+1].color.red();
		float R = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
		y1 = colorPoints[i].color.green();
		y2 = colorPoints[i+1].color.green();
		float G = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
		y1 = colorPoints[i].color.blue();
		y2 = colorPoints[i+1].color.blue();
		float B = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
		y1 = colorPoints[i].color.alpha();
		y2 = colorPoints[i+1].color.alpha();
		float A = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
		return Color(R,G,B,A);
	}
	// interpolate HSLA values
	float H = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
	y1 = colorPoints[i].color.saturationHSL();
	y2 = colorPoints[i+1].color.saturationHSL();
	float S = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
	y1 = colorPoints[i].color.lightness();
	y2 = colorPoints[i+1].color.lightness();
	float L = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
	y1 = colorPoints[i].color.alpha();
	y2 = colorPoints[i+1].color.alpha();
	float A = float(y1 + (y2 - y1) / (x2 - x1) * (x - x1));
	return Color::fromHSLA(H,S,L,A);
}

size_t Kystsoft::ValueColor::colorPointInterval(double value) const
{
	if (colorPoints.size() > 2)
	{
		size_t i = 0;
		for (; i < colorPoints.size() - 2; ++i)
			if (value <= colorPoints[i+1].value)
				return i;
		return i;
	}
	return 0;
}
