#include "LocationModule.h"

bool Kystsoft::LocationModule::isLocationMethodSupported(location_method_e method)
{
	if (!isLocationSupported())
		return false;
	switch (method)
	{
	case LOCATIONS_METHOD_GPS: return isGpsSupported();
	case LOCATIONS_METHOD_WPS: return isWpsSupported();
	case LOCATIONS_METHOD_HYBRID:
		return isGpsSupported() || isWpsSupported(); // TODO: Figure out if the hybrid mode requires both
	default:
		return false;
	}
}

void Kystsoft::LocationModule::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
}

void Kystsoft::LocationModule::start()
{
	if (createManager())
		manager->start();
}

void Kystsoft::LocationModule::stop()
{
	if (manager)
		manager->stop();
}

void Kystsoft::LocationModule::toggleStartStop()
{
	if (isStarted())
		stop();
	else
		start();
}

bool Kystsoft::LocationModule::createManager()
{
	// check if already created
	if (manager)
		return true;

	// perform system checks to be able to run on systems without location support
	if (!isLocationMethodSupported())
		return false;

	// create manager
	manager = std::make_unique<LocationManager>(locationMethod, interval_s);
	manager->enabledSignal().connect(this, &LocationModule::onLocationEnabled);
	manager->locationSignal().connect(this, &LocationModule::onLocationUpdated);
	return true;
}

void Kystsoft::LocationModule::onLocationUpdated(Location location)
{
	if (datum == GeodeticDatum::Geoid)
		location.altitude -= geoidHeight(location.latitude, location.longitude);
	locationSignl.emit(location);
}
