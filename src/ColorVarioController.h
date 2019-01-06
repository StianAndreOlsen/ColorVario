#ifndef KYSTSOFT_COLORVARIO_CONTROLLER_H
#define KYSTSOFT_COLORVARIO_CONTROLLER_H

//#include "BluetoothAdapter.h"
//#include "BluetoothAdvertiser.h"
#include "ColorVarioUserInterface.h"
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
	Settings settingsFromFiles();
	void saveAltitudeOffset();
	void goBack() { app.Lower(); }
	void quit() { saveAltitudeOffset(); app.Quit(); }
	void onPause(Dali::Application& application);
	void onResume(Dali::Application& application);
	void onContextLost(); // TODO: Remove after testing
	void onContextRegained(); // TODO: Remove after testing
	void onPageTapDetected();
	void onLocationUpdated(const Location& location);
	void onDisplayStateChanged(display_state_e state);
	void onAltitudeSignal(double altitude);
	void onAltitudeOffsetChanged(double offset);
	static Message aboutMessage();
	static Message variometerStartError();
	Dali::Application& app;
	UserInterface ui;
	SoundManager soundManager;
	Cpu cpu;
	Display display;
	bool displayLockPaused = false;
	std::unique_ptr<LocationManager> gps;
	time_t gpsStartTime = 0;
	double gpsBestAccuracy = 1e+6;
	Variometer vario;
//	BluetoothAdapter btAdapter;
//	std::unique_ptr<BluetoothAdvertiser> bleAdvertiser;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_CONTROLLER_H
