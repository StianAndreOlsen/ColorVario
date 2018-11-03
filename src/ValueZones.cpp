#include "ValueZones.h"

void Kystsoft::ValueZones::load(const Settings& settings, const std::string& section)
{
	clearZones();
	for (const auto& value : settings.values(section + ".onZone"))
	{
		Interval<double> zone;
		if (zone.load(value))
			addOnZone(zone);
	}
	for (const auto& value : settings.values(section + ".offZone"))
	{
		Interval<double> zone;
		if (zone.load(value))
			addOffZone(zone);
	}
}

bool Kystsoft::ValueZones::isOn(double value) const
{
	if (!onZones.empty() && !onZones.include(value))
		return false;
	if (!offZones.empty() && offZones.include(value))
		return false;
	return true;
}
