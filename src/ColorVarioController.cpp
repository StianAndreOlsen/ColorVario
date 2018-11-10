#include "ColorVarioController.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "FileFunctions.h"
#include "Storage.h"
#include "TimeFunctions.h"
#include <cmath>
#include <ctime>
#include <exception>

Kystsoft::ColorVario::Controller::Controller(Dali::Application& application)
	: app(application)
	, soundManager(SOUND_TYPE_MEDIA)
{
	// connect application signals
	app.InitSignal().Connect(this, &Controller::create);
	app.PauseSignal().Connect(this, &Controller::onPause);
	app.ResumeSignal().Connect(this, &Controller::onResume);

	// connect variometer signals
	vario.altitudeSignal().connect(this, &Controller::setAltitude);
	vario.altitudeSignal().connect(&ui, &UserInterface::setAltitude);
	vario.climbSignal().connect(&ui, &UserInterface::setClimb);

	// connect display signals
	display.stateChangedSignal().connect(this, &Controller::onDisplayStateChanged);

	// connect user interface signals
	ui.quitSignal().connect(this, &Controller::quit);
	ui.lockDisplaySignal().connect(&display, &Display::setLocked);
}

// The init signal is received only once during the application lifetime
void Kystsoft::ColorVario::Controller::create(Dali::Application& /*application*/)
{
	// exceptions does not propagate to main, probably because we are in a different thread
	try
	{
		createUi();
		load(settingsFromFile());

		// create, connect and start gps
		try
		{
			gps = std::make_unique<LocationManager>(LOCATIONS_METHOD_GPS);
			gps->loadGeoid(appSharedResourcePath() + "Geoid.dat");
			gps->startedSignal().connect(&ui, &UserInterface::setLocationEnabled);
			gps->locationSignal().connect(this, &Controller::onLocationUpdated);
			gps->start();
		}
		catch (std::exception& e)
		{
			dlog(DLOG_ERROR) << e.what();
			gps.reset(); // destroy gps
		}

		// start variometer
		try
		{
			vario.start();
		}
		catch (std::exception& e)
		{
			dlog(DLOG_ERROR) << e.what();
			ui.setErrorMessage("No pressure signal!");
		}

		// TODO: Create a function/class for this
		// start Bluetooth
		/*
		if (btAdapter.isEnabled())
		{
			try
			{
				bleAdvertiser = std::make_unique<BluetoothAdvertiser>();
//				bleAdvertiser->addServiceUuid("FFE0"); // TODO: First, I thought this was required but it's not. Consider removing!
				bleAdvertiser->start();
			}
			catch (std::exception& e)
			{
				dlog(DLOG_ERROR) << e.what();
			}
		}
		*/
	}
	catch (std::exception& e)
	{
		dlog(DLOG_FATAL) << e.what();
		app.Quit();
	}
	catch (...)
	{
		dlog(DLOG_FATAL) << "Unknown error";
		app.Quit();
	}
}

void Kystsoft::ColorVario::Controller::createUi()
{
	ui.create();

	// connect stage signals
	Dali::Stage stage = Dali::Stage::GetCurrent();
	stage.TouchSignal().Connect(this, &Controller::onTouch);
	stage.KeyEventSignal().Connect(this, &Controller::onKeyEvent);
	stage.WheelEventSignal().Connect(this, &Controller::onWheelEvent);
	stage.ContextLostSignal().Connect(this, &Controller::onContextLost);
	stage.ContextRegainedSignal().Connect(this, &Controller::onContextRegained);
}

void Kystsoft::ColorVario::Controller::load(const Settings& settings)
{
	// debug logging
	if (settings.value("Debug.enableLogging", false))
		setDebugLogFile(internalStorageRoot() + '/' + appName() + '/' + appName() + ".log");
	else
		setDebugLogFile(""); // terminate string stream buffering

	// sound
	soundManager.setVolume(settings.value("Sound.volume", -100.0) / 100);
	ui.setAudioMuted(settings.value("Sound.muted", false));

	// display
	display.setBrightness(settings.value("Display.brightness", -100.0) / 100);
	ui.setDisplayLocked(settings.value("Display.locked", false));

	// variometer
	vario.load(settings);

	// sampling intervals
	ui.setAltitudeSamplingInterval(vario.samplingInterval());
	ui.setClimbSamplingInterval(vario.samplingInterval());
	ui.setSpeedSamplingInterval(1); // TODO: Update with data from GPS

	// sounds, colors and labels
	ui.load(settings);
}

Kystsoft::Settings Kystsoft::ColorVario::Controller::settingsFromFile()
{
	std::string resourceDir = appSharedResourcePath();
	std::string storageDir = internalStorageRoot() + '/' + appName() + '/';
	std::string fileName = appName() + ".ini";
	std::string resourceFile = resourceDir + fileName;
	std::string storageFile = storageDir + fileName;
	FileFunctionsInitializer init;

	// make sure storage directory exists
	if (!isDirectory(storageDir) && !makeDirectory(storageDir))
		return Settings(resourceFile);

	// make sure storage file exists
	if (!fileExists(storageFile) && !copyFile(resourceFile, storageFile))
		return Settings(resourceFile);

	// load settings and check if storage file is up to date
	Settings settings(storageFile);
	if (settings.hasValue("SpeedLabel.unit")) // TODO: Update when ini file changes
		return settings;

	// backup existing storage file
	copyFile(storageFile, storageFile + '.' + currentDate());

	// copy resource file and reload settings
	if (!copyFile(resourceFile, storageFile))
		return Settings(resourceFile);
	return Settings(storageFile);
}

void Kystsoft::ColorVario::Controller::onPause(Dali::Application& /*application*/)
{
	if (display.isLocked())
	{
		display.unlock();
		displayLockPaused = true;
	}
	cpu.lock(); // no dad, I am not going to sleep!
}

void Kystsoft::ColorVario::Controller::onResume(Dali::Application& /*application*/)
{
	cpu.unlock(); // not even dad wants me to sleep while I am up and running
	if (displayLockPaused)
	{
		display.lock();
		displayLockPaused = false;
	}
}

void Kystsoft::ColorVario::Controller::onContextLost()
{
	dlog(DLOG_INFO) << "Context lost!";
}

void Kystsoft::ColorVario::Controller::onContextRegained()
{
	dlog(DLOG_INFO) << "Context regained!";
}

void Kystsoft::ColorVario::Controller::onTouch(const Dali::TouchData& touch)
{
	try
	{
		if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
		{
			if (!vario.isStarted())
				vario.start();
		}
	}
	catch (std::exception& e)
	{
		dlog(DLOG_ERROR) << e.what();
	}
}

void Kystsoft::ColorVario::Controller::onKeyEvent(const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Up)
	{
		// TODO: Go back to previous app instead of quitting
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
		    Dali::IsKey(event, Dali::DALI_KEY_BACK))
			app.GetWindow().Lower();
//			app.Quit(); // TODO: Remove!
	}
}

void Kystsoft::ColorVario::Controller::onWheelEvent(const Dali::WheelEvent& event)
{
	// TODO: Fix page changing code. This is just to check that the wheel event trigger this function.
//	if (event.z > 0)
//		app.Quit();
}

void Kystsoft::ColorVario::Controller::onLocationUpdated(const Location& location)
{
	// calibrate altimeter and set appropriate altitude label colors
	double accuracy = location.vertical;
	if (accuracy <= 0)
		accuracy = location.horizontal;
	if (accuracy <= gpsBestAccuracy)
	{
		gpsBestAccuracy = accuracy;
		vario.setCurrentAltitude(location.altitude);
		if (accuracy < 10)
			ui.setAltitudeTextColor(Dali::Color::WHITE);
		else if (accuracy < 100)
			ui.setAltitudeTextColor(Dali::Color::YELLOW);
	}

	// update user interface
	ui.setSpeed(location.speed);

	// consider stopping the gps
	double seconds = 0;
	if (gpsStartTime == 0)
		gpsStartTime = location.timestamp;
	else
		seconds = std::difftime(location.timestamp, gpsStartTime);
	if ((seconds > 60 && gpsBestAccuracy < 10) || seconds > 10 * 60)
		gps->stop(); // Note: Cannot destroy the GPS here, since this function is called from the GPS signal

	// TODO: Remove when Bluetooth implementation is finished
	// consider stopping the Bluetooth advertiser
//	if (seconds > 20)
//		bleAdvertiser->stop();
}

void Kystsoft::ColorVario::Controller::onDisplayStateChanged(display_state_e state)
{
	if (state == DISPLAY_STATE_NORMAL)
		app.GetWindow().Activate();
}

void Kystsoft::ColorVario::Controller::setAltitude(double altitude)
{
	if (gps && !gps->isStarted())
		gps.reset(); // destroy gps
}
