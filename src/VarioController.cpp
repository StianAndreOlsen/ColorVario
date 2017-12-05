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
//	stage.SetBackgroundColor(Dali::Color::GREEN);
	Dali::Vector2 stageSize = stage.GetSize();

	// create climb label
	climbLabel = Dali::Toolkit::TextLabel::New("0 m/s");
	climbLabel.SetSize(stageSize.width, stageSize.height / 4);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbLabel.SetPosition(0, stageSize.height / 4);
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE, 15);
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
//	climbLabel.SetBackgroundColor(Dali::Vector4(1.0f, 215.0f / 255.0f, 0.0f, 1.0f));
//	climbLabel.SetBackgroundColor(Dali::Color::BLACK);
	stage.Add(climbLabel);

	// create altitude label
	altitudeLabel = Dali::Toolkit::TextLabel::New("0 m");
	altitudeLabel.SetSize(stageSize.width, stageSize.height / 4);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudeLabel.SetPosition(0, stageSize.height / 2);
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::WHITE);
//	altitudeLabel.SetBackgroundColor(Dali::Vector4(1.0f, 165.0f / 255.0f, 0.0f, 1.0f));
//	altitudeLabel.SetBackgroundColor(Dali::Color::BLACK);
	stage.Add(altitudeLabel);

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

void Kystsoft::VarioController::setClimb(float climb)
{
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
