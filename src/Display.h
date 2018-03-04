#ifndef KYSTSOFT_DISPLAY_H
#define KYSTSOFT_DISPLAY_H

#include "Signal.h"
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
	const Signal<display_state_e>& stateChangedSignal() const { return stateChangedSignl; }
	float brightness() const;
	void setBrightness(float brightness);
	bool isLocked() const { return locked; }
	void setLocked(bool locked);
	void lock();
	void unlock();
	void toggleLockUnlock();
	const Signal<bool>& lockedSignal() const { return lockedSignl; }
private:
	void addStateChangedCallback();
	void removeStateChangedCallback();
	static void stateChanged(device_callback_e type, void* value, void* user_data);
	void onStateChanged(display_state_e state);
	int maxBrightness() const;
	Signal<display_state_e> stateChangedSignl;
	float initialBrightness = -1;
	bool locked = false;
	Signal<bool> lockedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DISPLAY_H
