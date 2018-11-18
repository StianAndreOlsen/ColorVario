#include "ValueColorPoint.h"

bool Kystsoft::ValueColorPoint::load(std::istream& is)
{
	float r = 0, g = 0, b = 0, a = 0;
	if (is >> value >> r >> g >> b >> a)
	{
		if (r > 1) r /= 255;
		if (g > 1) g /= 255;
		if (b > 1) b /= 255;
		if (a > 1) a /= 255;
		color = Color(r,g,b,a);
		return true;
	}
	return false;
}
