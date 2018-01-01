#include "VarioColorPoint.h"

bool Kystsoft::VarioColorPoint::load(std::istream& is)
{
	int r = 0, g = 0, b = 0;
	if (is >> climb >> r >> g >> b)
	{
		color = Color(r,g,b);
		return true;
	}
	return false;
}
