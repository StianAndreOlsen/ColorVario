#include "AltitudeLabel.h"
#include "UnitConversionFunctions.h"

void Kystsoft::AltitudeLabel::setUnit(const std::string& unit)
{
	if (unit == "ft" || unit == "feet")
	{
		ValueLabel::setUnit("ft");
		convertCallback().connect(&m_to_ft);
		setRoundToNearest(2);
	}
	else
	{
		ValueLabel::setUnit("m");
		convertCallback().disconnect();
		setRoundToNearest(1);
	}
}
