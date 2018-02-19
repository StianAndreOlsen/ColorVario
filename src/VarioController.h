#ifndef KYSTSOFT_VARIOCONTROLLER_H
#define KYSTSOFT_VARIOCONTROLLER_H

#include "Display.h"
#include "LocationManager.h"
#include "VarioAudio.h"
#include "VarioColor.h"
#include "Variometer.h"
#include <dali-toolkit/dali-toolkit.h>
#include <memory>

namespace Kystsoft {

class VarioController : public Dali::ConnectionTracker
{
public:
	VarioController(Dali::Application& application);
private:
	void create(Dali::Application& application);
	std::string validateSettings(const std::string& resourcePath);
	std::string loadVersion(const std::string& fileName);
	void saveVersion(const std::string& fileName);
	void copyFile(const std::string& fileName, const std::string& srcPath, const std::string& dstPath);
	void onTouch(const Dali::TouchData& touch);
	void onKeyEvent(const Dali::KeyEvent& event);
	void onLocationUpdated(const Location& location);
	void setBackgroundColor(const Color& color);
	void setClimb(float climb);
	void setAltitude(float altitude);
	Dali::Application& app;
	Dali::Toolkit::Control background;
	Dali::Toolkit::ImageView muteIcon;
	Dali::Toolkit::TextLabel climbLabel;
	Dali::Toolkit::TextLabel altitudeLabel;
	Dali::Toolkit::ImageView locationIcon;
	Display display;
	std::unique_ptr<LocationManager> gps;
	time_t gpsStartTime = 0;
	double gpsBestAccuracy = 1e+6;
	Variometer vario;
	VarioAudio audio;
	VarioColor color;
	Color currentColor;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOCONTROLLER_H
