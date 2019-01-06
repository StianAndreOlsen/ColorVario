#include "ColorVarioController.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "FileFunctions.h"
#include "Storage.h"
#include "TimeFunctions.h"
#include <cmath>
#include <ctime>
#include <exception>
#include <fstream>

Kystsoft::ColorVario::Controller::Controller(Dali::Application& application)
	: app(application)
	, soundManager(SOUND_TYPE_MEDIA)
{
	// connect application signals
	app.InitSignal().Connect(this, &Controller::create);
	app.PauseSignal().Connect(this, &Controller::onPause);
	app.ResumeSignal().Connect(this, &Controller::onResume);

	// connect variometer signals
	vario.altitudeSignal().connect(this, &Controller::onAltitudeSignal);
	vario.altitudeSignal().connect(&ui, &UserInterface::setAltitude);
	vario.climbSignal().connect(&ui, &UserInterface::setClimb);

	// connect display signals
	display.stateChangedSignal().connect(this, &Controller::onDisplayStateChanged);

	// connect user interface signals
	ui.goBackSignal().connect(this, &Controller::goBack);
	ui.lockDisplaySignal().connect(&display, &Display::setLocked);
	ui.quitSignal().connect(this, &Controller::quit);
	ui.pageTapDetectedSignal().connect(this, &Controller::onPageTapDetected);
	ui.altitudeOffsetChangedSignal().connect(this, &Controller::onAltitudeOffsetChanged);
}

// The init signal is received only once during the application lifetime
void Kystsoft::ColorVario::Controller::create(Dali::Application& /*application*/)
{
	// exceptions does not propagate to main, probably because we are in a different thread
	try
	{
		createUi();
		ui.addMessage(aboutMessage());
		load(settingsFromFiles());

		// create, connect and start gps
		// TODO: Consider encapsulating this into a LocationModule/GpsModule class where I also check if location/GPS services are available
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
			// TODO: Remove when finished testing!
//			throw std::runtime_error("Just testing!");
		}
		catch (std::exception& e)
		{
			dlog(DLOG_ERROR) << e.what();
			ui.addMessage(variometerStartError());
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

	// altitude offset
	ui.setAltitudeOffset(vario.altitudeOffset());

	// sampling intervals
	ui.setAltitudeSamplingInterval(vario.samplingInterval());
	ui.setClimbSamplingInterval(vario.samplingInterval());
	ui.setSpeedSamplingInterval(1); // TODO: Update with data from GPS

	// sounds, colors and labels
	ui.load(settings);
}

Kystsoft::Settings Kystsoft::ColorVario::Controller::settingsFromFiles()
{
	auto resourceDir = appSharedResourcePath();
	auto storageDir = internalStorageRoot() + '/' + appName() + '/';
	auto fileName = appName() + ".ini";
	auto resourceFile = resourceDir + fileName;
	auto storageFile = storageDir + fileName;
	auto offsetFile = storageDir + "AltitudeOffset.ini";
	FileFunctionsInitializer init;

	// first, load altitude offset
	Settings settings;
	if (fileExists(offsetFile))
		settings.load(offsetFile);

	// make sure storage directory exists
	if (!isDirectory(storageDir) && !makeDirectory(storageDir))
	{
		settings.load(resourceFile);
		return settings;
	}

	// make sure storage file exists
	if (!fileExists(storageFile) && !copyFile(resourceFile, storageFile))
	{
		settings.load(resourceFile);
		return settings;
	}

	// load settings and check if storage file is up to date
	settings.load(storageFile);
	if (settings.hasValue("SpeedLabel.unit")) // TODO: Update when ini file changes
		return settings;

	// backup existing storage file
	copyFile(storageFile, storageFile + '.' + currentDate());

	// copy resource file and reload settings
	if (!copyFile(resourceFile, storageFile))
		return Settings(resourceFile);
	return Settings(storageFile);
}

void Kystsoft::ColorVario::Controller::saveAltitudeOffset()
{
	auto storageDir = internalStorageRoot() + '/' + appName() + '/';
	auto offsetFile = storageDir + "AltitudeOffset.ini";
	std::ofstream os(offsetFile);
	os.precision(15);
	os << "[Variometer]\n";
	os << "altitudeOffset=" << vario.altitudeOffset() << ";\n";
	if (os.fail())
		dlog(DLOG_ERROR) << "Unable to save altitude offset to \"" << offsetFile << "\"";
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

void Kystsoft::ColorVario::Controller::onPageTapDetected()
{
	if (vario.isStarted())
		return;
	try
	{
		vario.start();
	}
	catch (std::exception& e)
	{
		dlog(DLOG_ERROR) << e.what();
		return; // error message already added
	}
	ui.removeMessage(variometerStartError());
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
		ui.setAltitudeAccuracy(accuracy);
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

void Kystsoft::ColorVario::Controller::onAltitudeSignal(double /*altitude*/)
{
	// destroy the gps if it's not running
	if (gps && !gps->isStarted())
		gps.reset(); // destroy gps
}

void Kystsoft::ColorVario::Controller::onAltitudeOffsetChanged(double offset)
{
	vario.setAltitudeOffset(offset);
}

Kystsoft::Message Kystsoft::ColorVario::Controller::aboutMessage()
{
	std::string about
	(
		"Developed by\n"      // hard line breaks are required
		"Kyrre Holm and\n"    // since TextLabel breaks lines
		"Stian Andre Olsen\n" // even at no-break spaces
		"\n"
		"Please visit facebook.com/ColorVariometer"
	);
	return Message::information("ColorVario 2.0.0", about);
}

Kystsoft::Message Kystsoft::ColorVario::Controller::variometerStartError()
{
	std::string text
	(
		"The variometer won't start. No signal from pressure sensor.\n"
		"\n"
		"Tap main page to attempt a restart."
	);
	return Message::error("Variometer Error", text);
}
