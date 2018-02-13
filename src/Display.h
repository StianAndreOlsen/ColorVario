#ifndef KYSTSOFT_DISPLAY_H
#define KYSTSOFT_DISPLAY_H

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
	int brightness() const;
	void setBrightness(int brightness);
	int maxBrightness() const;
	void setMaxBrightness() { setBrightness(maxBrightness()); }
	display_state_e state() const;
	bool isLocked() const { return locked; }
	void lock();
	void unlock();
private:
	void addStateChangedCallback();
	void removeStateChangedCallback();
	static void stateChanged(device_callback_e type, void* value, void* user_data);
	void onStateChanged(display_state_e state);
	int initialBrightness = 0;
	bool locked = false;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DISPLAY_H
