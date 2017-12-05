#include "VarioSoundPoint.h"
#include <cctype>

bool Kystsoft::VarioSoundPoint::load(std::istream& is, char delimiter /*= '\t'*/)
{
	if (std::isspace(delimiter))
	{
		if (is >> climb >> frequency >> period >> duty)
			return true;
	}
	else
	{
		if (is >> climb >> delimiter >> frequency >> delimiter >> period >> delimiter >> duty)
			return true;
	}
	return false;
}

void Kystsoft::VarioSoundPoint::save(std::ostream& os, char delimiter /*= '\t'*/) const
{
	os << climb << delimiter << frequency << delimiter << period << delimiter << duty;
}
