#ifndef KYSTSOFT_VARIOCONTROLLER_H
#define KYSTSOFT_VARIOCONTROLLER_H

#include "BluetoothAdapter.h"
#include "BluetoothLowEnergyAdvertiser.h"
#include "Cpu.h"
#include "Display.h"
#include "LocationManager.h"
#include "SoundManager.h"
#include "VarioAudio.h"
#include "VarioColor.h"
#include "Variometer.h"
#include <dali-toolkit/dali-toolkit.h>
#include <memory>

namespace Kystsoft {

class VarioController : public Dali::ConnectionTracker
{
public:
	VarioController(Dali::Application& application);
private:
	void create(Dali::Application& application);
	void createUi();
	void load(const Settings& settings);
	Settings settingsFromFile();
	void onPause(Dali::Application& application);
	void onResume(Dali::Application& application);
	void onContextLost(); // TODO: Remove after testing
	void onContextRegained(); // TODO: Remove after testing
	void onTouch(const Dali::TouchData& touch);
	void onKeyEvent(const Dali::KeyEvent& event);
	void onLocationUpdated(const Location& location);
	void onAudioMuted(bool muted);
	void onDisplayLocked(bool locked);
	void onDisplayStateChanged(display_state_e state);
	void setBackgroundColor(const Color& color);
	void setClimb(float climb);
	void setAltitude(float altitude);
	Dali::Application& app;
	Dali::Toolkit::Control background;
	Dali::Toolkit::Control iconBox;
	time_t lastIconBoxTouch = 0;
	Dali::Toolkit::ImageView mutedIcon;
	Dali::Toolkit::ImageView unmutedIcon;
	Dali::Toolkit::ImageView lockedIcon;
	Dali::Toolkit::ImageView unlockedIcon;
	Dali::Toolkit::TextLabel climbLabel;
	Dali::Toolkit::TextLabel altitudeLabel;
	Dali::Toolkit::ImageView locationIcon;
	std::string climbUnit = "m/s";
	std::string altitudeUnit = "m";
	bool showClimbUnit = true;
	bool showAltitudeUnit = true;
	SoundManager soundManager;
	Cpu cpu;
	Display display;
	bool displayLockPaused = false;
	std::unique_ptr<LocationManager> gps;
	time_t gpsStartTime = 0;
	double gpsBestAccuracy = 1e+6;
	Variometer vario;
	VarioAudio audio;
	VarioColor color;
	Color currentColor;
	BluetoothAdapter btAdapter;
	BluetoothLowEnergyAdvertiser bleAdvertiser;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCONTROLLER_H
