#include "VarioController.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "FileFunctions.h"
#include "Storage.h"
#include "TimeFunctions.h"
#include <cmath>
#include <ctime>
#include <exception>
#include <fstream>
#include <sstream>

Kystsoft::VarioController::VarioController(Dali::Application& application)
	: app(application)
	, soundManager(SOUND_TYPE_MEDIA)
{
	// connect application signals
	app.InitSignal().Connect(this, &VarioController::create);
	app.PauseSignal().Connect(this, &VarioController::onPause);
	app.ResumeSignal().Connect(this, &VarioController::onResume);

	// connect variometer signals
	vario.climbSignal().connect(this, &VarioController::setClimb);
	vario.climbSignal().connect(&audio, &VarioAudio::setClimb);
	vario.altitudeSignal().connect(this, &VarioController::setAltitude);

	// connect audio signals
	audio.mutedSignal().connect(this, &VarioController::onAudioMuted);

	// connect display signals
	display.lockedSignal().connect(this, &VarioController::onDisplayLocked);
	display.stateChangedSignal().connect(this, &VarioController::onDisplayStateChanged);
}

// The init signal is received only once during the application lifetime
void Kystsoft::VarioController::create(Dali::Application& /*application*/)
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
			gps->locationSignal().connect(this, &VarioController::onLocationUpdated);
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
			climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "No pressure");
			altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "signal!");
		}

		// TODO: Create a function for this
		// start Bluetooth
		if (btAdapter.isEnabled())
		{
			try
			{
				bleAdvertiser = std::make_unique<BluetoothLowEnergyAdvertiser>();
//				bleAdvertiser->addServiceUuid("FFE0"); // TODO: First, I thought this was required but it's not. Consider removing!
				bleAdvertiser->start();
			}
			catch (std::exception& e)
			{
				dlog(DLOG_ERROR) << e.what();
			}
		}
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

void Kystsoft::VarioController::createUi()
{
	// get a handle to the stage
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();

	// create background
	background = Dali::Toolkit::Control::New();
	background.SetSize(stageSize);
	background.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	background.SetPosition(0, 0);
	stage.Add(background);

	// create icon box
	float iconHeight = stageSize.height / 8;
	float iconSpacing = stageSize.width / 16;
	iconBox = Dali::Toolkit::Control::New();
	iconBox.SetSize(stageSize.width, iconHeight);
	iconBox.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	iconBox.SetPosition(stageSize.width / 2, stageSize.height / 8);
	background.Add(iconBox);
	lastIconBoxTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create muted icon
	mutedIcon = Dali::Toolkit::ImageView::New(resourceDir + "Muted.png");
	mutedIcon.SetSize(mutedIcon.GetWidthForHeight(iconHeight), iconHeight);
	mutedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	mutedIcon.SetPosition(stageSize.width / 2 - iconSpacing / 2, 0);
	mutedIcon.SetVisible(audio.isMuted());
	iconBox.Add(mutedIcon);

	// create unmuted icon
	unmutedIcon = Dali::Toolkit::ImageView::New(resourceDir + "Unmuted.png");
	unmutedIcon.SetSize(unmutedIcon.GetWidthForHeight(iconHeight), iconHeight);
	unmutedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	unmutedIcon.SetPosition(stageSize.width / 2 - iconSpacing / 2, 0);
	unmutedIcon.SetVisible(!audio.isMuted());
	iconBox.Add(unmutedIcon);

	// create locked icon
	lockedIcon = Dali::Toolkit::ImageView::New(resourceDir + "Locked.png");
	lockedIcon.SetSize(lockedIcon.GetWidthForHeight(iconHeight), iconHeight);
	lockedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	lockedIcon.SetPosition(stageSize.width / 2 + iconSpacing / 2, 0);
	lockedIcon.SetVisible(display.isLocked());
	iconBox.Add(lockedIcon);

	// create unlocked icon
	unlockedIcon = Dali::Toolkit::ImageView::New(resourceDir + "Unlocked.png");
	unlockedIcon.SetSize(unlockedIcon.GetWidthForHeight(iconHeight), iconHeight);
	unlockedIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	unlockedIcon.SetPosition(stageSize.width / 2 + iconSpacing / 2, 0);
	unlockedIcon.SetVisible(!display.isLocked());
	iconBox.Add(unlockedIcon);

	// create climb label
	climbLabel = Dali::Toolkit::TextLabel::New("Climb");
	climbLabel.SetSize(stageSize.width, stageSize.height / 4);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbLabel.SetPosition(0, stageSize.height / 4);
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE, 15);
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
	background.Add(climbLabel);

	// create altitude label
	altitudeLabel = Dali::Toolkit::TextLabel::New("Altitude");
	altitudeLabel.SetSize(stageSize.width, stageSize.height / 4);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudeLabel.SetPosition(0, stageSize.height / 2);
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::RED);
	background.Add(altitudeLabel);

	// create location icon
	locationIcon = Dali::Toolkit::ImageView::New(resourceDir + "Location.png");
	locationIcon.SetSize(locationIcon.GetWidthForHeight(iconHeight), iconHeight);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(stageSize.width / 2, stageSize.height * 3 / 4);
	locationIcon.SetVisible(false);
	background.Add(locationIcon);

	// connect stage signals
	stage.TouchSignal().Connect(this, &VarioController::onTouch);
	stage.KeyEventSignal().Connect(this, &VarioController::onKeyEvent);
	stage.ContextLostSignal().Connect(this, &VarioController::onContextLost);
	stage.ContextRegainedSignal().Connect(this, &VarioController::onContextRegained);
}

void Kystsoft::VarioController::load(const Settings& settings)
{
	// enable debug logging
	if (settings.value("Debug.enableLogging", false))
		setDebugLogFile(internalStorageRoot() + '/' + appName() + '/' + appName() + ".log");
	else
		setDebugLogFile(""); // terminate string stream buffering

	// set sound volume
	soundManager.setVolume(settings.value("Sound.volume", -100.0f) / 100);

	// control display
	display.setBrightness(settings.value("Display.brightness", -100.0f) / 100);
	display.setLocked(settings.value("Display.locked", false));

	// climb and altitude settings
	climbUnit = settings.value("Display.climbUnit");
	if (climbUnit != "fpm" && climbUnit != "hfpm" && climbUnit != "kn")
		climbUnit = "m/s";
	altitudeUnit = settings.value("Display.altitudeUnit");
	if (altitudeUnit != "ft")
		altitudeUnit = "m";
	showClimbUnit = settings.value("Display.showClimbUnit", true);
	showAltitudeUnit = settings.value("Display.showAltitudeUnit", true);

	// load variometer, sound and color settings
	vario.load(settings);
	audio.load(settings);
	color.load(settings);
}

Kystsoft::Settings Kystsoft::VarioController::settingsFromFile()
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
	if (settings.hasValue("Display.locked")) // TODO: Update when ini file changes
		return settings;

	// backup existing storage file
	copyFile(storageFile, storageFile + '.' + currentDate());

	// copy resource file and reload settings
	if (!copyFile(resourceFile, storageFile))
		return Settings(resourceFile);
	return Settings(storageFile);
}

void Kystsoft::VarioController::onPause(Dali::Application& /*application*/)
{
	if (display.isLocked())
	{
		display.unlock();
		displayLockPaused = true;
	}
	cpu.lock(); // no dad, I am not going to sleep!
}

void Kystsoft::VarioController::onResume(Dali::Application& /*application*/)
{
	cpu.unlock(); // not even dad wants me to sleep while I am up and running
	if (displayLockPaused)
	{
		display.lock();
		displayLockPaused = false;
	}
}

void Kystsoft::VarioController::onContextLost()
{
	dlog(DLOG_INFO) << "Context lost!";
}

void Kystsoft::VarioController::onContextRegained()
{
	dlog(DLOG_INFO) << "Context regained!";
}

void Kystsoft::VarioController::onTouch(const Dali::TouchData& touch)
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
						audio.toggleMuteUnmute();
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

void Kystsoft::VarioController::onKeyEvent(const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Down)
	{
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
		    Dali::IsKey(event, Dali::DALI_KEY_BACK))
			app.Quit();
	}
}

void Kystsoft::VarioController::onLocationUpdated(const Location& location)
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
			altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
		else if (accuracy < 100)
			altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::YELLOW);
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
	if (seconds > 20)
		bleAdvertiser->stop();
}

void Kystsoft::VarioController::onAudioMuted(bool muted)
{
	mutedIcon.SetVisible(muted);
	unmutedIcon.SetVisible(!muted);
}

void Kystsoft::VarioController::onDisplayLocked(bool locked)
{
	lockedIcon.SetVisible(locked);
	unlockedIcon.SetVisible(!locked);
}

void Kystsoft::VarioController::onDisplayStateChanged(display_state_e state)
{
	if (state == DISPLAY_STATE_NORMAL)
		app.GetWindow().Activate();

}

void Kystsoft::VarioController::setBackgroundColor(const Color& color)
{
	if (color == currentColor)
		return;
	currentColor = color;

	// radial gradient
	Dali::Property::Map map;
	map[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
	map[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
	map[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;
	Dali::Property::Array stopOffsets;
	stopOffsets.PushBack(0.00f);
	stopOffsets.PushBack(0.75f);
	stopOffsets.PushBack(1.00f);
	map[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
	Dali::Property::Array stopColors;
	stopColors.PushBack(Dali::Color::BLACK);
	stopColors.PushBack(Dali::Color::BLACK);
	stopColors.PushBack(color);
	map[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	background.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, map);
}

void Kystsoft::VarioController::setClimb(float climb)
{
	if (iconBox.IsVisible() && std::difftime(std::time(nullptr), lastIconBoxTouch) > 15)
		iconBox.SetVisible(false);

	setBackgroundColor(color(climb));

	std::ostringstream os;
	os.setf(std::ios::fixed);
	if (climbUnit == "fpm")
	{
		os.precision(0);
		os << std::round(climb * 60 / 0.3048f / 10) * 10;
	}
	else if (climbUnit == "hfpm")
	{
		os.precision(1);
		os << climb * 60 / 0.3048f / 100;
	}
	else if (climbUnit == "kn")
	{
		os.precision(1);
		os << climb * 60 * 60 / 1852;
	}
	else
	{
		os.precision(1);
		os << climb;
	}
	if (showClimbUnit)
		os << ' ' << climbUnit;
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}

void Kystsoft::VarioController::setAltitude(float altitude)
{
	if (gps && !gps->isStarted())
		gps.reset(); // destroy gps

	std::ostringstream os;
	os.setf(std::ios::fixed);
	os.precision(0);
	if (altitudeUnit == "ft")
		os << altitude / 0.3048f;
	else
		os << altitude;
	if (showAltitudeUnit)
		os << ' ' << altitudeUnit;
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}
