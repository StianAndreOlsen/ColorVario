#ifndef KYSTSOFT_VARIOCONTROLLER_H
#define KYSTSOFT_VARIOCONTROLLER_H

#include "Variometer.h"
//#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class VarioController : public Dali::ConnectionTracker
{
public:
	VarioController(Dali::Application& application);
	void Create(Dali::Application& application);
	void OnTouch(const Dali::TouchData& touch);
	void OnKeyEvent(const Dali::KeyEvent& event);
private:
	Dali::Application& app;
	Dali::Toolkit::TextLabel climbLabel;
	Dali::Toolkit::TextLabel altitudeLabel;
	Variometer vario;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCONTROLLER_H
