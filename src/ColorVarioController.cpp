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
	, gps(LOCATIONS_METHOD_GPS)
{
	// connect application signals
	app.InitSignal().Connect(this, &Controller::create);
	app.PauseSignal().Connect(this, &Controller::onPause);
	app.ResumeSignal().Connect(this, &Controller::onResume);

	// connect user interface signals
	ui.goBackSignal().connect(this, &Controller::goBack);
	ui.lockDisplaySignal().connect(&display, &Display::setLocked);
	ui.quitSignal().connect(this, &Controller::quit);
	ui.pageTapDetectedSignal().connect(this, &Controller::onPageTapDetected);
	ui.altitudeOffsetChangedSignal().connect(this, &Controller::onAltitudeOffsetChanged);

	// connect privilege signals
	privilegeManager.permissionGrantedSignal().connect(this, &Controller::onPermissionGranted);
	privilegeManager.permissionDeniedSignal().connect(this, &Controller::onPermissionDenied);

	// connect display signals
	display.stateChangedSignal().connect(this, &Controller::onDisplayStateChanged);

	// connect variometer signals
	vario.altitudeSignal().connect(&ui, &UserInterface::setAltitude);
	vario.climbSignal().connect(&ui, &UserInterface::setClimb);

	// connect gps signals
	gps.enabledSignal().connect(this, &Controller::onLocationEnabled);
	gps.locationSignal().connect(this, &Controller::onLocationUpdated);

	// create timers
	gpsStartOrStopTimer = Dali::Timer::New(5000);
	gpsStartOrStopTimer.TickSignal().Connect(this, &Controller::startOrStopGps);
}

void Kystsoft::ColorVario::Controller::create(Dali::Application& /*application*/)
{
	// exceptions does not propagate to main, probably because we are in a different thread
	try
	{
		// create user interface
		ui.create();
		ui.addMessage(aboutMessage());

		// get required permissions
		if (!privilegeManager.getPermission("privilege/mediastorage"))
			ui.addMessage(storageNotPermittedWarning());
		if (gps.isLocationMethodSupported())
			privilegeManager.getPermission("privilege/location");

		// load settings and data
		load(settingsFromFiles());
		gps.loadGeoid(appSharedResourcePath() + "Geoid.dat");

		// start variometer, gps and bluetooth
		startVariometer();
		if (gps.isLocationMethodSupported())
		{
			startGps();
			gpsStartOrStopTimer.Start();
		}
		else
			ui.addMessage(gpsNotSupportedWarning());
		startBluetooth();
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
	ui.setSpeedSamplingInterval(gps.samplingInterval());

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

bool Kystsoft::ColorVario::Controller::startVariometer()
{
	if (vario.isStarted())
		return true;
	try
	{
		vario.start();
	}
	catch (std::exception& e)
	{
		dlog(DLOG_ERROR) << e.what();
		ui.addMessage(variometerStartError());
		return false;
	}
	ui.removeMessage(variometerStartError());
	return true;
}

bool Kystsoft::ColorVario::Controller::startGps()
{
	if (gps.isStarted())
		return true;
	if (!gps.isLocationMethodSupported())
		return false;
	if (!gps.hasPermission())
	{
		ui.addMessage(gpsNotPermittedWarning());
		return false;
	}
	ui.removeMessage(gpsNotPermittedWarning());
	gpsStartTime = 0;
	gpsStopped = false;
	try
	{
		gps.start();
	}
	catch (std::exception& e)
	{
		// TODO: Consider logging everything except "GPS not enabled"
		// for now; skip logging to avoid filling up the log when GPS is disabled
//		dlog(DLOG_ERROR) << e.what();
		ui.setLocationEnabled(false);
		ui.addMessage(gpsNotAvailableWarning());
		return false;
	}
	ui.setLocationEnabled(true);
	ui.removeMessage(gpsNotAvailableWarning());
	return true;
}

bool Kystsoft::ColorVario::Controller::startBluetooth()
{
	return false;
	// TODO: Create a BluetoothModule class a la LocationModule
/*
	if (btAdapter.isEnabled())
	{
		try
		{
			bleAdvertiser = std::make_unique<BluetoothAdvertiser>();
//			bleAdvertiser->addServiceUuid("FFE0"); // TODO: First, I thought this was required but it's not. Consider removing!
			bleAdvertiser->start();
		}
		catch (std::exception& e)
		{
			dlog(DLOG_ERROR) << e.what();
		}
	}
*/
}

bool Kystsoft::ColorVario::Controller::startOrStopGps()
{
	if (gps.isStarted())
	{
		if (ui.isGpsRequired())
			gpsStartTime = std::time(nullptr); // fake start time to avoid stopping it immediately when not required
		else if (gpsStartTime > 0)
		{
			auto seconds = std::difftime(std::time(nullptr), gpsStartTime);
			if ((seconds > 60 && gpsBestAccuracy < 10) || seconds > 10 * 60)
			{
				gpsStopped = true;
				gps.stop();
			}
		}
	}
	else if (ui.isGpsRequired() || !gpsStopped)
		startGps();
	return true;
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

Kystsoft::Message Kystsoft::ColorVario::Controller::storageNotPermittedWarning()
{
	std::string text
	(
		"ColorVario is not allowed to read from and write to the local media storage.\n"
		"\n"
		"If you deny access to the local media storage, ColorVario will not be able to:\n"
		"\n"
		"1. Read your custom settings from ColorVario.ini.\n"
		"\n"
		"2. Save and restore the altitude offset.\n"
		"\n"
		"3. Write debug information to ColorVario.log."
	);
	return Message::warning("Storage Not Permitted", text);
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

Kystsoft::Message Kystsoft::ColorVario::Controller::gpsNotSupportedWarning()
{
	std::string text
	(
		"GPS location is not supported by your device.\n"
		"\n"
		"Ground speed will never be displayed and altitude will not get calibrated. "
		"However, climb values are valid and you can manually calibrate the altitude."
	);
	return Message::warning("GPS Not Supported", text);
}

Kystsoft::Message Kystsoft::ColorVario::Controller::gpsNotPermittedWarning()
{
	std::string text
	(
		"ColorVario is not allowed to use the GPS.\n"
		"\n"
		"If you deny access to the GPS, "
		"ground speed will never be displayed and altitude will not get calibrated. "
		"However, climb values are valid and you can manually calibrate the altitude."
	);
	return Message::warning("GPS Not Permitted", text);
}

Kystsoft::Message Kystsoft::ColorVario::Controller::gpsNotAvailableWarning()
{
	std::string text
	(
		"Please check that GPS location is enabled.\n"
		"\n"
		"If connected to a phone, enable the GPS on the phone. "
		"If operating standalone, enable the GPS on the watch."
	);
	return Message::warning("GPS Not Available", text);
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

void Kystsoft::ColorVario::Controller::onDisplayStateChanged(display_state_e state)
{
	if (state == DISPLAY_STATE_NORMAL)
		app.GetWindow().Activate();
}

void Kystsoft::ColorVario::Controller::onPermissionGranted(std::string privilege)
{
	if (privilege == "privilege/mediastorage")
	{
		ui.removeMessage(storageNotPermittedWarning());
		load(settingsFromFiles()); // reload settings
	}
	else if (privilege == "privilege/location")
		startGps();
}

void Kystsoft::ColorVario::Controller::onPermissionDenied(std::string privilege)
{
	if (privilege == "privilege/mediastorage")
		ui.addMessage(storageNotPermittedWarning());
}

void Kystsoft::ColorVario::Controller::onLocationEnabled(bool enabled)
{
	ui.setLocationEnabled(enabled);
	if (enabled)
	{
		ui.removeMessage(gpsNotAvailableWarning());
		gpsStartTime = std::time(nullptr);
		gpsStopped = false;
	}
	else if (!gpsStopped)
	{
		ui.addMessage(gpsNotAvailableWarning());
		gpsStartTime = 0;
	}
}

void Kystsoft::ColorVario::Controller::onLocationUpdated(Location location)
{
	// calibrate altimeter and set appropriate altitude label colors
	auto accuracy = location.vertical;
	if (accuracy <= 0)
		accuracy = location.horizontal;
	if (accuracy <= gpsBestAccuracy)
	{
		gpsBestAccuracy = accuracy;
		vario.setCurrentAltitude(location.altitude);
		ui.setAltitudeAccuracy(accuracy);
	}

	// update speed label
	ui.setSpeed(location.speed);

	// TODO: Remove when Bluetooth implementation is finished
	// consider stopping the Bluetooth advertiser
//	if (seconds > 20)
//		bleAdvertiser->stop();
}
