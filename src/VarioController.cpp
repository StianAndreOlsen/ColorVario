#include "VarioController.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "FileFunctions.h"
#include "Storage.h"
#include "TimeFunctions.h"
#include <cmath>
#include <exception>
#include <fstream>
#include <sstream>

Kystsoft::VarioController::VarioController(Dali::Application& application)
	: app(application)
	, soundManager(SOUND_TYPE_MEDIA)
{
	// connect the application's init signal
	app.InitSignal().Connect(this, &VarioController::create);
}

// The init signal is received only once during the application lifetime
void Kystsoft::VarioController::create(Dali::Application& /*application*/)
{
	// exceptions does not propagate to main, probably because we are in a different thread
	try
	{
		// create user interface
		createUi();

		// load settings from file
		load(settingsFromFile());

		// connect variometer signals
		vario.climbSignal().connect(this, &VarioController::setClimb);
		vario.climbSignal().connect(&audio, &VarioAudio::setClimb);
		vario.altitudeSignal().connect(this, &VarioController::setAltitude);

		// create, connect and start gps
		try
		{
			gps = std::make_unique<LocationManager>(LOCATIONS_METHOD_GPS, 5);
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
	background.SetSize(stageSize.width, stageSize.height);
	background.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	background.SetPosition(0, 0);
	stage.Add(background);

	// create mute icon
	std::string resourceDir = appSharedResourcePath();
	muteIcon = Dali::Toolkit::ImageView::New(resourceDir + "Mute.png");
	muteIcon.SetSize(muteIcon.GetWidthForHeight(stageSize.height / 8), stageSize.height / 8);
	muteIcon.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	muteIcon.SetPosition(stageSize.width / 2, stageSize.height / 4);
	muteIcon.SetVisible(audio.isMuted());
	audio.mutedSignal().connect(dynamic_cast<Dali::Actor*>(&muteIcon), &Dali::Actor::SetVisible);
	background.Add(muteIcon);

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
	locationIcon.SetSize(locationIcon.GetWidthForHeight(stageSize.height / 8), stageSize.height / 8);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(stageSize.width / 2, stageSize.height * 3 / 4);
	locationIcon.SetVisible(false);
	background.Add(locationIcon);

	// connect stage signals
	stage.TouchSignal().Connect(this, &VarioController::onTouch);
	stage.KeyEventSignal().Connect(this, &VarioController::onKeyEvent);
}

void Kystsoft::VarioController::load(const Settings& settings)
{
	// enable debug logging
	if (settings.value("Debug.enableLogging", false))
		setDebugLogFile(internalStorageRoot() + "/ColorVario/ColorVario.log");
	else
		setDebugLogFile(""); // terminate string stream buffering

	// set sound volume
	float volume = settings.value("Sound.volume", -1.0f);
	if (volume >= 0)
		soundManager.setVolume(volume / 100);

	// control display
	float brightness = settings.value("Display.brightness", -1.0f);
	if (brightness >= 0)
		display.setBrightness(brightness / 100);
	if (settings.value("Display.alwaysOn", false))
		display.lock();

	// climb and altitude settings
	climbUnit = settings.value("Display.climbUnit");
	if (climbUnit != "fpm" && climbUnit != "hfpm")
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
	std::string storageDir = internalStorageRoot() + "/ColorVario/";
	std::string fileName = "ColorVario.ini";
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
	if (settings.hasValue("Sound.volume")) // TODO: Update when ColorVario.ini changes
		return settings;

	// backup existing storage file
	copyFile(storageFile, storageFile + '.' + currentDate());

	// copy resource file and reload settings
	if (!copyFile(resourceFile, storageFile))
		return Settings(resourceFile);
	return Settings(storageFile);
}

void Kystsoft::VarioController::onTouch(const Dali::TouchData& touch)
{
	try
	{
		if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
		{
			if (vario.isStarted())
				audio.toggleMuteUnmute();
			else
				vario.start();
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
	// TODO: Figure out a better place to destroy the GPS, or get used to this! ;-)
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
