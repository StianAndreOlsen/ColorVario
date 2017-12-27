#include "VarioController.h"
#include "AppFunctions.h"
#include "dlog.h"
#include "LocationManager.h"
#include <exception>
#include <sstream>

Kystsoft::VarioController::VarioController(Dali::Application& application)
	: app(application)
{
	// connect the application's init signal
	app.InitSignal().Connect(this, &VarioController::create);
}

// The init signal is received only once during the application lifetime
void Kystsoft::VarioController::create(Dali::Application& application)
{
	// exceptions does not propagate to main, probably because we are in a different thread
	try
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

		// create climb label
		climbLabel = Dali::Toolkit::TextLabel::New("0 m/s");
		climbLabel.SetSize(stageSize.width, stageSize.height / 4);
		climbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		climbLabel.SetPosition(0, stageSize.height / 4);
		climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE, 15);
		climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
		climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
		climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
		background.Add(climbLabel);

		// create altitude label
		altitudeLabel = Dali::Toolkit::TextLabel::New("0 m");
		altitudeLabel.SetSize(stageSize.width, stageSize.height / 4);
		altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		altitudeLabel.SetPosition(0, stageSize.height / 2);
		altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
		altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
		altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::RED);
		background.Add(altitudeLabel);

		// connect stage signals
		stage.TouchSignal().Connect(this, &VarioController::onTouch);
		stage.KeyEventSignal().Connect(this, &VarioController::onKeyEvent);

		// connect variometer signals
		vario.climbSignal().connect(this, &VarioController::setClimb);
		vario.climbSignal().connect(&audio, &VarioAudio::setClimb);
		vario.altitudeSignal().connect(this, &VarioController::setAltitude);

		// create, connect and start gps
		try
		{
			gps = std::make_unique<LocationManager>(LOCATIONS_METHOD_GPS);
			gps->loadGeoid(appSharedResourcePath() + "Geoid.dat");
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
	vario.setCurrentAltitude(location.altitude);
	if (location.vertical < 10)
	{
		altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
		gps->stop(); // TODO: Figure out a way of destroying the gps when not needed
//		gps.reset(); // destroy gps // Note: Cannot destroy gps here, since this is called from the gps signal
	}
	else if (location.vertical < 100)
	{
		altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::YELLOW);
	}
	// TODO: Destroy gps after a certain amount of time
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
	stopOffsets.PushBack(0.0f);
	stopOffsets.PushBack(0.7f);
	stopOffsets.PushBack(1.0f);
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
	os.precision(1);
	os << climb << " m/s";
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}

void Kystsoft::VarioController::setAltitude(float altitude)
{
	std::ostringstream os;
	os.setf(std::ios::fixed);
	os.precision(0);
	os << altitude << " m";
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}
