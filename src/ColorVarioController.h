#ifndef KYSTSOFT_COLORVARIO_CONTROLLER_H
#define KYSTSOFT_COLORVARIO_CONTROLLER_H

#include "AltitudeLabel.h"
#include "BluetoothAdapter.h"
#include "BluetoothAdvertiser.h"
#include "ClimbAudio.h"
#include "ClimbLabel.h"
#include "ClimbRing.h"
#include "Cpu.h"
#include "Display.h"
#include "LocationManager.h"
#include "SoundManager.h"
#include "Variometer.h"
#include <memory>

namespace Kystsoft {
namespace ColorVario {

class Controller : public Dali::ConnectionTracker
{
public:
	Controller(Dali::Application& application);
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
	void onWheelEvent(const Dali::WheelEvent& event);
	void onLocationUpdated(const Location& location);
	void onAudioMuted(bool muted);
	void onDisplayLocked(bool locked);
	void onDisplayStateChanged(display_state_e state);
	void setClimb(double climb);
	void setAltitude(double altitude);
	Dali::Application& app;
	ClimbRing climbRing;
	Dali::Toolkit::Control iconBox;
	time_t lastIconBoxTouch = 0;
	Dali::Toolkit::ImageView mutedIcon;
	Dali::Toolkit::ImageView unmutedIcon;
	Dali::Toolkit::ImageView lockedIcon;
	Dali::Toolkit::ImageView unlockedIcon;
	ClimbLabel climbLabel;
	AltitudeLabel altitudeLabel;
	Dali::Toolkit::ImageView locationIcon;
	SoundManager soundManager;
	Cpu cpu;
	Display display;
	bool displayLockPaused = false;
	std::unique_ptr<LocationManager> gps;
	time_t gpsStartTime = 0;
	double gpsBestAccuracy = 1e+6;
	Variometer vario;
	ClimbAudio climbAudio;
//	BluetoothAdapter btAdapter;
//	std::unique_ptr<BluetoothAdvertiser> bleAdvertiser;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_CONTROLLER_H
