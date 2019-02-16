#ifndef KYSTSOFT_LOCATIONMANAGER_H
#define KYSTSOFT_LOCATIONMANAGER_H

#include "Location.h"
#include "Signal.h"

namespace Kystsoft {

class LocationManager
{
public:
	LocationManager(location_method_e method, int interval = 1);
	LocationManager(const LocationManager& other) = delete;
	LocationManager& operator=(const LocationManager& rhs) = delete;
	~LocationManager() noexcept;
	operator location_manager_h() const { return manager; }
	bool isStarted() const { return started; }
	void setStarted(bool started);
	void start();
	void stop();
	void toggleStartStop();
	const auto& enabledSignal() const { return enabledSignl; }
	const auto& locationSignal() const { return locationSignl; }
private:
	void create(location_method_e method);
	void destroy();
	void setServiceStateChangedCallback();
	void unsetServiceStateChangedCallback();
	void setPositionUpdatedCallback(int interval);
	void unsetPositionUpdatedCallback();
	static void serviceStateChanged(location_service_state_e state, void* user_data);
	static void positionUpdated(double latitude, double longitude, double altitude, time_t timestamp, void* user_data);
	void onServiceStateChanged(location_service_state_e state);
	void onPositionUpdated();
	location_manager_h manager = nullptr;
	bool started = false;
	Signal<bool> enabledSignl;
	Signal<Location> locationSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_LOCATIONMANAGER_H
