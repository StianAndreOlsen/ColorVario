#ifndef KYSTSOFT_VALUEZONES_H
#define KYSTSOFT_VALUEZONES_H

#include "Intervals.h"
#include "Settings.h"

namespace Kystsoft {

class ValueZones
{
public:
	void load(const Settings& settings, const std::string& section);
	void addOnZone(const Interval<double>& zone) { onZones.push_back(zone); }
	void addOffZone(const Interval<double>& zone) { offZones.push_back(zone); }
	void addOnZone(double lower, double upper) { onZones.push_back(Interval<double>(lower, upper)); }
	void addOffZone(double lower, double upper) { offZones.push_back(Interval<double>(lower, upper)); }
	void clearOnZones() { onZones.clear(); }
	void clearOffZones() { offZones.clear(); }
	void clearZones() { clearOnZones(); clearOffZones(); }
	bool isOn(double value) const;
	bool isOff(double value) const { return !isOn(value); }
private:
	Intervals<double> onZones;
	Intervals<double> offZones;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEZONES_H
