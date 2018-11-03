#include "ValueColorPoint.h"

bool Kystsoft::ValueColorPoint::load(std::istream& is)
{
	int r = 0, g = 0, b = 0;
	if (is >> value >> r >> g >> b)
	{
		color = Color(r,g,b);
		return true;
	}
	return false;
}
