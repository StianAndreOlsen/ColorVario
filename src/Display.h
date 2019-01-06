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
	Display(const Display& other) = delete;
	Display& operator=(const Display& rhs) = delete;
	~Display() noexcept;
	display_state_e state() const;
	const auto& stateChangedSignal() const { return stateChangedSignl; }
	double brightness() const;
	void setBrightness(double brightness);
	bool isLocked() const { return locked; }
	void setLocked(bool locked);
	void lock();
	void unlock();
	void toggleLockUnlock();
private:
	void addStateChangedCallback();
	void removeStateChangedCallback();
	static void stateChanged(device_callback_e type, void* value, void* user_data);
	void onStateChanged(display_state_e state);
	int maxBrightness() const;
	Signal<display_state_e> stateChangedSignl;
	double initialBrightness = -1;
	double wantedBrightness = -1;
	bool locked = false;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DISPLAY_H
