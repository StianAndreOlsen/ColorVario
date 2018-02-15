#include "LocationManager.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::LocationManager::LocationManager(location_method_e method, int interval /*= 1*/)
{
	create(method);
	try
	{
		setPositionUpdatedCallback(interval);
	}
	catch (...)
	{
		destroy();
		throw;
	}
}

Kystsoft::LocationManager::~LocationManager() noexcept
{
	try { stop(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { unsetPositionUpdatedCallback(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::LocationManager::start()
{
	if (started)
		return;
	int error = location_manager_start(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_start", error);
	startedSignl.emit(started = true);
}

void Kystsoft::LocationManager::stop()
{
	if (!started)
		return;
	int error = location_manager_stop(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_stop", error);
	startedSignl.emit(started = false);
}

void Kystsoft::LocationManager::toggleStartStop()
{
	if (started)
		stop();
	else
		start();
}

void Kystsoft::LocationManager::create(location_method_e method)
{
	int error = location_manager_create(method, &manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_create", error);
}

void Kystsoft::LocationManager::destroy()
{
	int error = location_manager_destroy(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_destroy", error);
	manager = nullptr;
}

void Kystsoft::LocationManager::setPositionUpdatedCallback(int interval)
{
	int error = location_manager_set_position_updated_cb(manager, positionUpdated, interval, this);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_set_position_updated_cb", error);
}

void Kystsoft::LocationManager::unsetPositionUpdatedCallback()
{
	int error = location_manager_unset_position_updated_cb(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_unset_position_updated_cb", error);
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
	// TODO: Debug and check the difference between the two functions below
//	int error = location_manager_get_location(manager,
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
