#ifndef KYSTSOFT_VARIOCONTROLLER_H
#define KYSTSOFT_VARIOCONTROLLER_H

#include "VarioColor.h"
#include "Variometer.h"
#include "VarioSound.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class VarioController : public Dali::ConnectionTracker
{
public:
	VarioController(Dali::Application& application);
private:
	void create(Dali::Application& application);
	void onTouch(const Dali::TouchData& touch);
	void onKeyEvent(const Dali::KeyEvent& event);
	void setBackgroundColor(const Color& color);
	void setClimb(float climb);
	void setAltitude(float altitude);
	Dali::Application& app;
	Dali::Toolkit::Control background;
	Dali::Toolkit::TextLabel climbLabel;
	Dali::Toolkit::TextLabel altitudeLabel;
	Variometer vario;
	VarioSound sound;
	VarioColor color;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCONTROLLER_H
