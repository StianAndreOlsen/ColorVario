#include "ValueSoundPoint.h"

bool Kystsoft::ValueSoundPoint::load(std::istream& is)
{
	if (is >> value >> frequency >> period >> duty)
		return true;
	return false;
}
