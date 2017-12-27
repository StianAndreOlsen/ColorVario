#include "LocationManager.h"
#include "TizenError.h"

Kystsoft::LocationManager::LocationManager(location_method_e method, int interval /*= 1*/)
{
	int error = location_manager_create(method, &manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_create", error);

	error = location_manager_set_position_updated_cb(manager, positionUpdated, interval, this);
	if (error != LOCATIONS_ERROR_NONE)
	{
		location_manager_destroy(manager);
		manager = nullptr;
		throw TizenError("location_manager_set_position_updated_cb", error);
	}
}

void Kystsoft::LocationManager::start()
{
	if (started)
		return;
	int error = location_manager_start(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_start", error);
	started = true;
}

void Kystsoft::LocationManager::stop()
{
	if (!started)
		return;
	int error = location_manager_stop(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_stop", error);
	started = false;
}

void Kystsoft::LocationManager::toggleStartStop()
{
	if (started)
		stop();
	else
		start();
}

void Kystsoft::LocationManager::positionUpdated(double /*latitude*/, double /*longitude*/, double /*altitude*/, time_t /*timestamp*/, void* user_data)
{
	LocationManager* manager = static_cast<LocationManager*>(user_data);
	if (manager != nullptr)
		manager->onPositionUpdated();
}

void Kystsoft::LocationManager::onPositionUpdated()
{
	Location location;
	int error = location_manager_get_last_location(manager,
		&location.altitude,
		&location.latitude,
		&location.longitude,
		&location.climb,
		&location.direction,
		&location.speed,
		&location.level,
		&location.horizontal,
		&location.vertical,
		&location.timestamp
	);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_get_last_location", error);
	location.speed /= 3.6f; // convert from km/h to m/s
	location.climb /= 3.6f; // convert from km/h to m/s
	if (datum == GeodeticDatum::Geoid)
		location.altitude -= geoid.height(location.latitude, location.longitude);
	locationSignl.emit(location);
}
