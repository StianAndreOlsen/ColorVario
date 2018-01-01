#include "VarioSoundPoint.h"

bool Kystsoft::VarioSoundPoint::load(std::istream& is)
{
	if (is >> climb >> frequency >> period >> duty)
		return true;
	return false;
}
