#include "Display.h"
#include "dlog.h"
#include "TizenError.h"
#include <device/power.h>

Kystsoft::Display::Display()
{
	// save initial brightness
	initialBrightness = brightness();

	// register state changed callback
	int error = device_add_callback(DEVICE_CALLBACK_DISPLAY_STATE, displayStateChanged, this);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_add_callback", error);
}

Kystsoft::Display::~Display() noexcept
{
	try
	{
		// restore initial brightness
		if (initialBrightness != brightness()) // TODO: Consider skipping this test
			setBrightness(initialBrightness);

		// unlock if locked
		unlock();
	}
	// TODO: Consider skipping the error handling
	catch (std::exception& e)
	{
		dlog(DLOG_ERROR) << e.what();
	}
	catch (...)
	{
		dlog(DLOG_ERROR) << "Display::~Display: Unknown error";
	}
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

void Kystsoft::Display::lock()
{
	if (locked)
		return;
	int error = device_power_request_lock(POWER_LOCK_DISPLAY, 0);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_request_lock", error);
	locked = true;
}

void Kystsoft::Display::unlock()
{
	if (!locked)
		return;
	int error = device_power_release_lock(POWER_LOCK_DISPLAY);
	if (error != DEVICE_ERROR_NONE)
		throw TizenError("device_power_release_lock", error);
	locked = false;
}

void Kystsoft::Display::displayStateChanged(device_callback_e type, void* value, void* user_data)
{
	Display* display = static_cast<Display*>(user_data);
	if (display != nullptr)
		display->onDisplayStateChanged(static_cast<display_state_e>(reinterpret_cast<int>(value)));
}

void Kystsoft::Display::onDisplayStateChanged(display_state_e state)
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
