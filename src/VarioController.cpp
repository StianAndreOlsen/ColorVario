#include "VarioController.h"
#include <dali-toolkit/dali-toolkit.h>

Kystsoft::VarioController::VarioController(Dali::Application& application)
	: app(application)
{
	// connect to the application's init signal
	app.InitSignal().Connect(this, &VarioController::Create);
}

// The init signal is received only once during the application lifetime
void Kystsoft::VarioController::Create(Dali::Application& application)
{
	// Get a handle to the stage
	Dali::Stage stage = Dali::Stage::GetCurrent();
	stage.SetBackgroundColor(Dali::Color::GREEN);
	Dali::Vector2 stageSize = stage.GetSize();

	// Create climb label
	climbLabel = Dali::Toolkit::TextLabel::New("0 m/s");
	climbLabel.SetSize(stageSize.width, stageSize.height / 4);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbLabel.SetPosition(0, stageSize.height / 4);
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE, 15);
//	climbLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLUE);
//	climbLabel.SetBackgroundColor(Dali::Color::YELLOW);
	stage.Add(climbLabel);
	vario.SetClimbLabel(&climbLabel);

	// Create altitude label
	altitudeLabel = Dali::Toolkit::TextLabel::New("1000 m");
	altitudeLabel.SetSize(stageSize.width, stageSize.height / 4);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudeLabel.SetPosition(0, stageSize.height / 2);
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
//	altitudeLabel.SetBackgroundColor(Dali::Color::BLUE);
	stage.Add(altitudeLabel);
	vario.SetAltitudeLabel(&altitudeLabel);
//	altitudeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Hurra!");

	// Start variometer
//	vario.Start();

	// Connect to touch & key event signals
	stage.TouchSignal().Connect(this, &VarioController::OnTouch);
	stage.KeyEventSignal().Connect(this, &VarioController::OnKeyEvent);
}

void Kystsoft::VarioController::OnTouch(const Dali::TouchData& touch)
{
	app.Quit();
}

void Kystsoft::VarioController::OnKeyEvent(const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Down)
	{
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
			Dali::IsKey(event, Dali::DALI_KEY_BACK))
			app.Quit();
		if (Dali::IsKey(event, Dali::DALI_KEY_BACKSPACE))
		{
			if (vario.IsStarted())
				vario.Stop();
			else
				vario.Start();
		}
	}
}
