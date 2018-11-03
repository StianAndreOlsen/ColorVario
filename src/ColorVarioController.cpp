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
	vario.climbSignal().connect(this, &Controller::setClimb);
	vario.altitudeSignal().connect(this, &Controller::setAltitude);

	// connect audio signals
	climbAudio.mutedSignal().connect(this, &Controller::onAudioMuted);

	// connect display signals
	display.lockedSignal().connect(this, &Controller::onDisplayLocked);
	display.stateChangedSignal().connect(this, &Controller::onDisplayStateChanged);
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
			gps->startedSignal().connect(dynamic_cast<Dali::Actor*>(&locationIcon), &Dali::Actor::SetVisible);
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
			// TODO: Consider showing a separate error label
			climbLabel.setText("No pressure");
			altitudeLabel.setText("signal!");
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
	// get a handle to the stage
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();

	// create climb ring
	climbRing = ClimbRing::New();
	climbRing.SetSize(stageSize);
	climbRing.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbRing.SetPosition(0, 0);
	stage.Add(climbRing);

	// create icon box
	float iconHeight = stageSize.height / 8;
	float iconSpacing = stageSize.width / 16;
	iconBox = Dali::Toolkit::Control::New();
	iconBox.SetSize(stageSize.width, iconHeight);
	iconBox.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	iconBox.SetPosition(stageSize.width / 2, stageSize.height / 8);
	climbRing.Add(iconBox);
	lastIconBoxTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create muted icon
	mutedIcon = Dali::Toolkit::ImageView::New(resourceDir + "AudioMuted.png");
	mutedIcon.SetSize(mutedIcon.GetWidthForHeight(iconHeight), iconHeight);
	mutedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	mutedIcon.SetPosition(stageSize.width / 2 - iconSpacing / 2, 0);
	mutedIcon.SetVisible(climbAudio.isMuted());
	iconBox.Add(mutedIcon);

	// create unmuted icon
	unmutedIcon = Dali::Toolkit::ImageView::New(resourceDir + "AudioUnmuted.png");
	unmutedIcon.SetSize(unmutedIcon.GetWidthForHeight(iconHeight), iconHeight);
	unmutedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	unmutedIcon.SetPosition(stageSize.width / 2 - iconSpacing / 2, 0);
	unmutedIcon.SetVisible(!climbAudio.isMuted());
	iconBox.Add(unmutedIcon);

	// create locked icon
	lockedIcon = Dali::Toolkit::ImageView::New(resourceDir + "DisplayLocked.png");
	lockedIcon.SetSize(lockedIcon.GetWidthForHeight(iconHeight), iconHeight);
	lockedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	lockedIcon.SetPosition(stageSize.width / 2 + iconSpacing / 2, 0);
	lockedIcon.SetVisible(display.isLocked());
	iconBox.Add(lockedIcon);

	// create unlocked icon
	unlockedIcon = Dali::Toolkit::ImageView::New(resourceDir + "DisplayUnlocked.png");
	unlockedIcon.SetSize(unlockedIcon.GetWidthForHeight(iconHeight), iconHeight);
	unlockedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	unlockedIcon.SetPosition(stageSize.width / 2 + iconSpacing / 2, 0);
	unlockedIcon.SetVisible(!display.isLocked());
	iconBox.Add(unlockedIcon);

	// create climb label
	climbLabel = ClimbLabel::New("Climb");
	climbLabel.SetSize(stageSize.width, stageSize.height / 4);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbLabel.SetPosition(0, stageSize.height / 4);
	climbLabel.setPointSize(15);
	climbLabel.setHorizontalAlignment("CENTER");
	climbLabel.setVerticalAlignment("CENTER");
	climbLabel.setTextColor(Dali::Color::WHITE);
	climbRing.Add(climbLabel);

	// create altitude label
	altitudeLabel = AltitudeLabel::New("Altitude");
	altitudeLabel.SetSize(stageSize.width, stageSize.height / 4);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudeLabel.SetPosition(0, stageSize.height / 2);
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setTextColor(Dali::Color::RED);
	climbRing.Add(altitudeLabel);

	// create location icon
	locationIcon = Dali::Toolkit::ImageView::New(resourceDir + "Location.png");
	locationIcon.SetSize(locationIcon.GetWidthForHeight(iconHeight), iconHeight);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(stageSize.width / 2, stageSize.height * 3 / 4);
	locationIcon.SetVisible(false);
	climbRing.Add(locationIcon);

	// connect stage signals
	stage.TouchSignal().Connect(this, &Controller::onTouch);
	stage.KeyEventSignal().Connect(this, &Controller::onKeyEvent);
	stage.WheelEventSignal().Connect(this, &Controller::onWheelEvent);
	stage.ContextLostSignal().Connect(this, &Controller::onContextLost);
	stage.ContextRegainedSignal().Connect(this, &Controller::onContextRegained);
}

void Kystsoft::ColorVario::Controller::load(const Settings& settings)
{
	// enable debug logging
	if (settings.value("Debug.enableLogging", false))
		setDebugLogFile(internalStorageRoot() + '/' + appName() + '/' + appName() + ".log");
	else
		setDebugLogFile(""); // terminate string stream buffering

	// set sound volume
	soundManager.setVolume(settings.value("Sound.volume", -100.0) / 100);

	// control display
	display.setBrightness(settings.value("Display.brightness", -100.0) / 100);
	display.setLocked(settings.value("Display.locked", false));

	// variometer settings
	vario.load(settings);

	// sound settings
	climbAudio.setSamplingInterval(vario.samplingInterval());
	climbAudio.load(settings);

	// color settings
	climbRing.setSamplingInterval(vario.samplingInterval());
	climbRing.load(settings);

	// label settings
	climbLabel.setSamplingInterval(vario.samplingInterval());
	climbLabel.load(settings);
	altitudeLabel.setSamplingInterval(vario.samplingInterval());
	altitudeLabel.load(settings);
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
			else if (touch.GetScreenPosition(0).y < climbLabel.GetCurrentPosition().y)
			{
				lastIconBoxTouch = std::time(nullptr);
				if (!iconBox.IsVisible())
					iconBox.SetVisible(true);
				else
				{
					if (touch.GetScreenPosition(0).x < climbLabel.GetCurrentSize().width / 2)
						climbAudio.toggleMuteUnmute();
					else
						display.toggleLockUnlock();
				}
			}
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
//			app.GetWindow().Lower();
			app.Quit();
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
			altitudeLabel.setTextColor(Dali::Color::WHITE);
		else if (accuracy < 100)
			altitudeLabel.setTextColor(Dali::Color::YELLOW);
	}

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

void Kystsoft::ColorVario::Controller::onAudioMuted(bool muted)
{
	mutedIcon.SetVisible(muted);
	unmutedIcon.SetVisible(!muted);
}

void Kystsoft::ColorVario::Controller::onDisplayLocked(bool locked)
{
	lockedIcon.SetVisible(locked);
	unlockedIcon.SetVisible(!locked);
}

void Kystsoft::ColorVario::Controller::onDisplayStateChanged(display_state_e state)
{
	if (state == DISPLAY_STATE_NORMAL)
		app.GetWindow().Activate();
}

void Kystsoft::ColorVario::Controller::setClimb(double climb)
{
	if (iconBox.IsVisible() && std::difftime(std::time(nullptr), lastIconBoxTouch) > 15)
		iconBox.SetVisible(false);

	climbAudio.setClimb(climb);
	climbRing.setClimb(climb);
	climbLabel.setClimb(climb);
}

void Kystsoft::ColorVario::Controller::setAltitude(double altitude)
{
	if (gps && !gps->isStarted())
		gps.reset(); // destroy gps

	altitudeLabel.setAltitude(altitude);
}
