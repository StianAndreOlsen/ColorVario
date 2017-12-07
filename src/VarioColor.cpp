#include "VarioColor.h"
#include <algorithm>

Kystsoft::VarioColor::VarioColor()
{
	// TODO: Consider adding default color points
}

void Kystsoft::VarioColor::sortColorPoints()
{
	std::sort(colorPoints.begin(), colorPoints.end());
}

Kystsoft::Color Kystsoft::VarioColor::color(float climb) const
{
	if (colorPoints.empty())
		return defaultColor(climb);
	if (colorPoints.size() < 2)
		return colorPoints[0].color;
	size_t i = colorPointInterval(climb);
	float x = climb;
	float x1 = colorPoints[i].climb;
	float x2 = colorPoints[i+1].climb;
	// TODO: Choose the best color model for color interpolation
	float y1 = colorPoints[i].color.hue();
	float y2 = colorPoints[i+1].color.hue();
	float H = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
	y1 = colorPoints[i].color.saturationHSL();
	y2 = colorPoints[i+1].color.saturationHSL();
	float S = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
	y1 = colorPoints[i].color.lightness();
	y2 = colorPoints[i+1].color.lightness();
	float L = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
	y1 = colorPoints[i].color.alpha();
	y2 = colorPoints[i+1].color.alpha();
	float A = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
	return Color::fromHSLA(H,S,L,A);
}

Kystsoft::Color Kystsoft::VarioColor::defaultColor(float climb)
{
	// hue(climb) (https://en.wikipedia.org/wiki/Logistic_function)
	float k = 0.5f; // curve steepness

	// blue - green - red
	float H = 300 / (1 + std::exp(k * climb)) - 30;
	if (H < 0)
		H += 360;

	// red - yellow - green
//	float H = 120 / (1 + std::exp(-k * climb));

	// HSL color model
//	float S = 1.0f; // saturation
//	float L = 0.5f; // lightness
//	return Color::fromHSLA(H,S,L);

	// HSV color model
//	float S = 1.0f; // saturation
//	float V = 0.5f; // value
//	return Color::fromHSVA(H,S,V);

	// HCY color model
	float C = 1.0f; // chroma
	float Y = 0.5f; // luma
	return Color::fromHCYA(H,C,Y);
}

size_t Kystsoft::VarioColor::colorPointInterval(float climb) const
{
	if (colorPoints.size() > 2)
	{
		size_t i = 0;
		for (; i < colorPoints.size() - 2; ++i)
			if (climb <= colorPoints[i+1].climb)
				return i;
		return i;
	}
	return 0;
}
