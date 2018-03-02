#ifndef KYSTSOFT_DISPLAY_H
#define KYSTSOFT_DISPLAY_H

#include "AppManager.h"
#include <device/callback.h>
#include <device/display.h>

namespace Kystsoft {

class Display
{
public:
	Display();
	~Display() noexcept;
	Display(const Display& other) = delete;
	Display& operator=(const Display& rhs) = delete;
	display_state_e state() const;
	void setState(display_state_e state);
	float brightness() const;
	void setBrightness(float brightness);
	bool isLocked() const { return locked; }
	void setLocked(bool locked);
	void lock();
	void unlock();
private:
	void addStateChangedCallback();
	void removeStateChangedCallback();
	static void stateChanged(device_callback_e type, void* value, void* user_data);
	void onStateChanged(display_state_e state);
	int maxBrightness() const;
	float initialBrightness = -1;
	bool locked = false;
	AppManager appManager;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DISPLAY_H
