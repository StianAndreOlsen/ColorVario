#include "ClimbLabel.h"
#include "UnitConversionFunctions.h"

void Kystsoft::ClimbLabel::setUnit(const std::string& unit)
{
	if (unit == "km/h" || unit == "kmph")
	{
		ValueLabel::setUnit("km/h");
		convertCallback().connect(&mps_to_kmph);
		setRoundToNearest(0.5);
	}
	else if (unit == "ft/s" || unit == "fps")
	{
		ValueLabel::setUnit("ft/s");
		convertCallback().connect(&mps_to_fps);
		setRoundToNearest(0.2);
	}
	else if (unit == "fpm" || unit == "ft/min")
	{
		ValueLabel::setUnit("fpm");
		convertCallback().connect(&mps_to_fpm);
		setRoundToNearest(20);
	}
	else if (unit == "hfpm" || unit == "100ft/min")
	{
		ValueLabel::setUnit("hfpm");
		convertCallback().connect(&mps_to_hfpm);
		setRoundToNearest(0.2);
	}
	else if (unit == "kn" || unit == "knots")
	{
		ValueLabel::setUnit("kn");
		convertCallback().connect(&mps_to_kn);
		setRoundToNearest(0.2);
	}
	else if (unit == "mph" || unit == "mi/h")
	{
		ValueLabel::setUnit("mph");
		convertCallback().connect(&mps_to_mph);
		setRoundToNearest(0.2);
	}
	else
	{
		ValueLabel::setUnit("m/s");
		convertCallback().disconnect();
		setRoundToNearest(0.1);
	}
}
