#include "VarioColorPoint.h"
#include <cctype>

bool Kystsoft::VarioColorPoint::load(std::istream& is, char delimiter /*= '\t'*/)
{
	if (std::isspace(delimiter))
	{
		if (is >> climb >> color.r >> color.g >> color.b >> color.a)
			return true;
	}
	else
	{
		if (is >> climb >> delimiter >> color.r >> delimiter >> color.g >> delimiter >> color.b >> delimiter >> color.a)
			return true;
	}
	return false;
}

void Kystsoft::VarioColorPoint::save(std::ostream& os, char delimiter /*= '\t'*/) const
{
	os << climb << delimiter << color.r << delimiter << color.g << delimiter << color.b << delimiter << color.a;
}
