#include "ValueColorPoint.h"
#include <sstream>

bool Kystsoft::ValueColorPoint::load(const std::string& str)
{
	std::istringstream is(str);
	std::string rgba;
	if (is >> value && std::getline(is, rgba))
	{
		color = Color::fromRGBA(rgba);
		return true;
	}
	return false;
}
