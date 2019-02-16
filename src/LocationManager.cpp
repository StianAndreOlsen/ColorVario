#include "LocationManager.h"
#include "dlog.h"
#include "TizenError.h"
#include <cmath>

Kystsoft::LocationManager::LocationManager(location_method_e method, int interval /*= 1*/)
{
	create(method);
	try
	{
		setServiceStateChangedCallback();
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
	try { unsetServiceStateChangedCallback(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::LocationManager::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
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

void Kystsoft::LocationManager::setServiceStateChangedCallback()
{
	int error = location_manager_set_service_state_changed_cb(manager, serviceStateChanged, this);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_set_service_state_changed_cb", error);
}

void Kystsoft::LocationManager::unsetServiceStateChangedCallback()
{
	int error = location_manager_unset_service_state_changed_cb(manager);
	if (error != LOCATIONS_ERROR_NONE)
		throw TizenError("location_manager_unset_service_state_changed_cb", error);
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

void Kystsoft::LocationManager::serviceStateChanged(location_service_state_e state, void* user_data)
{
	LocationManager* manager = static_cast<LocationManager*>(user_data);
	if (manager)
		manager->onServiceStateChanged(state);
}

void Kystsoft::LocationManager::positionUpdated(double /*latitude*/, double /*longitude*/, double /*altitude*/, time_t /*timestamp*/, void* user_data)
{
	LocationManager* manager = static_cast<LocationManager*>(user_data);
	if (manager)
		manager->onPositionUpdated();
}

void Kystsoft::LocationManager::onServiceStateChanged(location_service_state_e state)
{
	enabledSignl.emit(state == LOCATIONS_SERVICE_ENABLED);
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
	location.speed /= 3.6; // convert from km/h to m/s
	location.climb /= 3.6; // convert from km/h to m/s
	location.direction = std::fmod(location.direction, 360.0); // make sure it's within [0, 360)
	locationSignl.emit(location);
}
