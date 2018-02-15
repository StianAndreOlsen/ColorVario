#include "Display.h"
#include "dlog.h"
#include "TizenError.h"
#include <device/power.h>

Kystsoft::Display::Display()
{
	setState(DISPLAY_STATE_NORMAL); // required since initialBrightness becomes 0 if the screen is off
	addStateChangedCallback();
	try
	{
		initialBrightness = brightness();
	}
	catch (...)
	{
		removeStateChangedCallback();
		throw;
	}
}

Kystsoft::Display::~Display() noexcept
{
	try { unlock(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { setBrightness(initialBrightness); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { removeStateChangedCallback(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

int Kystsoft::Display::brightness() const
{
	int brightness = 0;
	int error = device_display_get_brightness(0, &brightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_brightness", error);
	return brightness;
}

void Kystsoft::Display::setBrightness(int brightness)
{
	int error = device_display_set_brightness(0, brightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_set_brightness", error);
}

int Kystsoft::Display::maxBrightness() const
{
	int maxBrightness = 0;
	int error = device_display_get_max_brightness(0, &maxBrightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_max_brightness", error);
	return maxBrightness;
}

display_state_e Kystsoft::Display::state() const
{
	display_state_e state = DISPLAY_STATE_NORMAL;
	int error = device_display_get_state(&state);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_state", error);
	return state;
}

void Kystsoft::Display::setState(display_state_e state)
{
	// TODO: Find a replacement since this function is deprecated
	int error = device_display_change_state(state);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_change_state", error);
}

void Kystsoft::Display::lock()
{
	if (locked)
		return;
	// TODO: Find a replacement since this usage is deprecated
	int error = device_power_request_lock(POWER_LOCK_DISPLAY, 0);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_request_lock", error);
	locked = true;
}

void Kystsoft::Display::unlock()
{
	if (!locked)
		return;
	// TODO: Find a replacement since this usage is deprecated
	int error = device_power_release_lock(POWER_LOCK_DISPLAY);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_release_lock", error);
	locked = false;
}

void Kystsoft::Display::addStateChangedCallback()
{
	int error = device_add_callback(DEVICE_CALLBACK_DISPLAY_STATE, stateChanged, this);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_add_callback", error);
}

void Kystsoft::Display::removeStateChangedCallback()
{
	int error = device_remove_callback(DEVICE_CALLBACK_DISPLAY_STATE, stateChanged);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_remove_callback", error);
}

void Kystsoft::Display::stateChanged(device_callback_e type, void* value, void* user_data)
{
	Display* display = static_cast<Display*>(user_data);
	if (display != nullptr)
		display->onStateChanged(static_cast<display_state_e>(reinterpret_cast<int>(value)));
}

void Kystsoft::Display::onStateChanged(display_state_e state)
{
	if (locked)
	{
		// unlock when display is turned off
		if (state == DISPLAY_STATE_SCREEN_OFF)
		{
			unlock();
			locked = true; // keep locked flag to reset lock when display is turned on
		}

		// reset lock when display is turned on
		else if (state == DISPLAY_STATE_NORMAL)
		{
			locked = false;
			lock();
		}
	}
}
