#ifndef KYSTSOFT_COLORVARIO_CONTROLLER_H
#define KYSTSOFT_COLORVARIO_CONTROLLER_H

//#include "BluetoothAdapter.h"
//#include "BluetoothAdvertiser.h"
#include "ColorVarioUserInterface.h"
#include "Cpu.h"
#include "Display.h"
#include "LocationModule.h"
#include "PrivilegeManager.h"
#include "SoundManager.h"
#include "Variometer.h"

namespace Kystsoft {
namespace ColorVario {

class Controller : public Dali::ConnectionTracker
{
public:
	Controller(Dali::Application& application);
private:
	void create(Dali::Application& application);
	void load(const Settings& settings);
	Settings settingsFromFiles();
	void saveAltitudeOffset();
	void goBack() { app.Lower(); }
	void quit() { saveAltitudeOffset(); app.Quit(); }
	bool startVariometer();
	bool startGps();
	bool startBluetooth();
	bool startOrStopGps();
	static Message aboutMessage();
	static Message storageNotPermittedWarning();
	static Message variometerStartError();
	static Message gpsNotSupportedWarning();
	static Message gpsNotPermittedWarning();
	static Message gpsNotAvailableWarning();
	void onPause(Dali::Application& application);
	void onResume(Dali::Application& application);
	void onPageTapDetected() { startVariometer(); }
	void onDisplayStateChanged(display_state_e state);
	void onAltitudeOffsetChanged(double offset) { vario.setAltitudeOffset(offset); }
	void onPermissionGranted(std::string privilege);
	void onPermissionDenied(std::string privilege);
	void onLocationEnabled(bool enabled);
	void onLocationUpdated(Location location);
	Dali::Application& app;
	UserInterface ui;
	PrivilegeManager privilegeManager;
	SoundManager soundManager;
	Cpu cpu;
	Display display;
	bool displayLockPaused = false;
	Variometer vario;
	LocationModule gps;
	time_t gpsStartTime = 0;
	bool gpsStopped = false;
	double gpsBestAccuracy = 1e+6;
	Dali::Timer gpsStartOrStopTimer;
//	BluetoothAdapter btAdapter;
//	std::unique_ptr<BluetoothAdvertiser> bleAdvertiser;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_CONTROLLER_H
