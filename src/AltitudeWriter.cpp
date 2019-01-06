#include "AltitudeWriter.h"
#include "UnitConversionFunctions.h"

void Kystsoft::AltitudeWriter::setUnit(const std::string& unit)
{
	if (unit == "ft" || unit == "feet")
	{
		ValueWriter::setUnit("ft");
		convertCallback().connect(&m_to_ft);
		setRoundToNearest(2);
	}
	else
	{
		ValueWriter::setUnit("m");
		convertCallback().disconnect();
		setRoundToNearest(1);
	}
}
