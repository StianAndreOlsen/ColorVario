#ifndef KYSTSOFT_LOCATIONMANAGER_H
#define KYSTSOFT_LOCATIONMANAGER_H

#include "GeodeticDatum.h"
#include "Geoid.h"
#include "Location.h"
#include "Signal.h"

namespace Kystsoft {

class LocationManager
{
public:
	LocationManager(location_method_e method, int interval = 1);
	LocationManager(const LocationManager& other) = delete;
	LocationManager& operator=(const LocationManager& rhs) = delete;
	~LocationManager() noexcept { location_manager_destroy(manager); }
	operator location_manager_h() const { return manager; }
	bool loadGeoid(const std::string& fileName) { return geoid.load(fileName); }
	GeodeticDatum geodeticDatum() const { return datum; }
	void setGeodeticDatum(GeodeticDatum geodeticDatum) { datum = geodeticDatum; }
	bool isStarted() const { return started; }
	void start();
	void stop();
	void toggleStartStop();
	using LocationSignal = Signal<const Location&>; // const location --> all slots get the same location
	const LocationSignal& locationSignal() const { return locationSignl; }
private:
	static void positionUpdated(double latitude, double longitude, double altitude, time_t timestamp, void* user_data);
	void onPositionUpdated();
	Geoid geoid;
	GeodeticDatum datum = GeodeticDatum::Geoid;
	location_manager_h manager = nullptr;
	bool started = false;
	LocationSignal locationSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_LOCATIONMANAGER_H
