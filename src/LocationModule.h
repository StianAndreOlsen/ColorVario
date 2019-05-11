#ifndef KYSTSOFT_LOCATIONMODULE_H
#define KYSTSOFT_LOCATIONMODULE_H

#include "GeodeticDatum.h"
#include "Geoid.h"
#include "LocationManager.h"
#include "PrivilegeManager.h"
#include "SystemInfo.h"
#include <memory>

namespace Kystsoft {

class LocationModule
{
public:
	LocationModule(location_method_e method, int interval = 1)
		: locationMethod(method), interval_s(interval) {}
	LocationModule(const LocationModule& other) = delete;
	LocationModule& operator=(const LocationModule& rhs) = delete;
	~LocationModule() noexcept { manager.reset(); } // make sure the manager dies before us TODO: Figure out why this is required, discuss with Einar.
	static bool isLocationSupported() { return SystemInfo::platformBool("feature/location"); }
	static bool isGpsSupported() { return SystemInfo::platformBool("feature/location.gps"); }
	static bool isWpsSupported() { return SystemInfo::platformBool("feature/location.wps"); }
	static bool isLocationMethodSupported(location_method_e method);
	bool isLocationMethodSupported() const { return isLocationMethodSupported(locationMethod); }
	static bool hasPermission() { return PrivilegeManager::hasPermission("privilege/location"); }
	int samplingInterval() const { return interval_s; }
	bool loadGeoid(const std::string& fileName) { return geoid.load(fileName); }
	double geoidHeight(double latitude, double longitude) const { return geoid.height(latitude, longitude); }
	GeodeticDatum geodeticDatum() const { return datum; }
	void setGeodeticDatum(GeodeticDatum geodeticDatum) { datum = geodeticDatum; }
	bool isStarted() const { return manager ? manager->isStarted() : false; }
	void setStarted(bool started);
	void start();
	void stop();
	void toggleStartStop();
	const auto& enabledSignal() const { return enabledSignl; }
	const auto& locationSignal() const { return locationSignl; }
private:
	bool createManager();
	void onLocationEnabled(bool enabled) { enabledSignl.emit(enabled); }
	void onLocationUpdated(Location location);
	location_method_e locationMethod = LOCATIONS_METHOD_NONE;
	int interval_s = 1; // seconds
	std::unique_ptr<LocationManager> manager;
	Geoid geoid;
	GeodeticDatum datum = GeodeticDatum::Geoid;
	Signal<bool> enabledSignl;
	Signal<Location> locationSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_LOCATIONMODULE_H
