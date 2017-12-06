#include "VarioController.h"
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
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
	background.Add(altitudeLabel);

	// connect stage signals
	stage.TouchSignal().Connect(this, &VarioController::onTouch);
	stage.KeyEventSignal().Connect(this, &VarioController::onKeyEvent);

	// connect variometer signals
	vario.climbSignal().connect(this, &VarioController::setClimb);
	vario.climbSignal().connect(&sound, &VarioSound::setClimb);
	vario.altitudeSignal().connect(this, &VarioController::setAltitude);

	// start variometer
	vario.start();
}

void Kystsoft::VarioController::onTouch(const Dali::TouchData& touch)
{
	if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
		sound.toggleMuteUnmute();
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

void Kystsoft::VarioController::setBackgroundColor(const Color& color)
{
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
	stopColors.PushBack(color.withLightness(0.08f));
	stopColors.PushBack(color.withLightness(0.12f));
	stopColors.PushBack(color);
	map[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	background.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, map);
}

void Kystsoft::VarioController::setClimb(float climb)
{
	setBackgroundColor(color(climb));

	std::ostringstream os;
	os.setf(std::ios::fixed);
	os.precision(2); // TODO: Reduce to 1?
	os << climb << " m/s";
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}

void Kystsoft::VarioController::setAltitude(float altitude)
{
	std::ostringstream os;
	os.setf(std::ios::fixed);
	os.precision(2); // TODO: Reduce to 0?
	os << altitude << " m";
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
}
