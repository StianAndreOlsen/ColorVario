#include "Display.h"
#include "dlog.h"
#include "TizenError.h"
#include <device/power.h>

Kystsoft::Display::Display()
{
	addStateChangedCallback();
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

display_state_e Kystsoft::Display::state() const
{
	display_state_e state = DISPLAY_STATE_NORMAL;
	int error = device_display_get_state(&state);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_state", error);
	return state;
}

double Kystsoft::Display::brightness() const
{
	int ibrightness = 0;
	int error = device_display_get_brightness(0, &ibrightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_brightness", error);
	return double(ibrightness) / maxBrightness();
}

void Kystsoft::Display::setBrightness(double brightness)
{
	if (brightness < 0)
		return;
	if (brightness > 1)
		brightness = 1;
	if (initialBrightness < 0)
	{
		initialBrightness = this->brightness();
		if (initialBrightness == 0)
		{
			// store the wanted brightness and try again when the display is turned on
			initialBrightness = -1;
			wantedBrightness = brightness;
			return;
		}
	}
	int ibrightness = int(brightness * maxBrightness() + 0.5);
	if (ibrightness < 1)
		ibrightness = 1;
	int error = device_display_set_brightness(0, ibrightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_set_brightness", error);
}

void Kystsoft::Display::setLocked(bool locked)
{
	if (locked)
		lock();
	else
		unlock();
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

void Kystsoft::Display::toggleLockUnlock()
{
	if (locked)
		unlock();
	else
		lock();
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
	if (display)
		display->onStateChanged(static_cast<display_state_e>(reinterpret_cast<int>(value)));
}

void Kystsoft::Display::onStateChanged(display_state_e state)
{
	if (wantedBrightness >= 0 && state == DISPLAY_STATE_NORMAL)
	{
		setBrightness(wantedBrightness);
		wantedBrightness = -1;
	}
	stateChangedSignl.emit(state);
}

int Kystsoft::Display::maxBrightness() const
{
	int maxBrightness = 0;
	int error = device_display_get_max_brightness(0, &maxBrightness);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_display_get_max_brightness", error);
	return maxBrightness;
}
