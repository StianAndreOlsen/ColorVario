#ifndef KYSTSOFT_COLORVARIO_USERINTERFACE_H
#define KYSTSOFT_COLORVARIO_USERINTERFACE_H

#include "AltitudeAudio.h"
#include "AltitudeLabel.h"
#include "AltitudePage.h"
#include "AltitudeRing.h"
#include "ClimbAudio.h"
#include "ClimbLabel.h"
#include "ClimbPage.h"
#include "ClimbRing.h"
#include "ColorVarioMenu.h"
#include "PageView.h"
#include "Signal.h"
#include "SpeedLabel.h"
#include "SpeedPage.h"
#include <ctime>

namespace Kystsoft {
namespace ColorVario {

class UserInterface : public Dali::ConnectionTracker
{
public:
	UserInterface() {}
	UserInterface(const UserInterface& other) = delete;
	UserInterface& operator=(const UserInterface& rhs) = delete;
	void create();
	void load(const Settings& settings);
	bool isMenuVisible() const { return menu.GetCurrentPosition().y > -menu.GetTargetSize().height; }
	void setMenuVisible(bool visible);
	void showMenu() { setMenuVisible(true); }
	void hideMenu() { setMenuVisible(false); }
	void showOrHideMenu(float vy);
	void translateMenu(float dy);
	void setAltitudeSamplingInterval(double interval);
	void setClimbSamplingInterval(double interval);
	void setSpeedSamplingInterval(double interval);
	void setAltitudeAccuracy(double accuracy);
	void setAltitude(double altitude);
	void setClimb(double climb);
	void setSpeed(double speed);
	void setErrorMessage(const std::string& message);
	void setAudioMuted(bool muted);
	void setDisplayLocked(bool locked);
	void setBluetoothEnabled(bool enabled);
	void setBluetoothConnected(bool connected);
	void setLocationEnabled(bool enabled) { menu.locationIcon().SetVisible(enabled); }
	const Signal<>& quitSignal() const { return quitSignl; }
	const Signal<bool>& lockDisplaySignal() const { return lockDisplaySignl; }
	const Signal<bool>& enableBluetoothSignal() const { return enableBluetoothSignl; }
private:
	void createPageLayer();
	void createMenuLayer();
	void onTapGesture(Dali::Actor actor, const Dali::TapGesture& gesture);
	void onPanGesture(Dali::Actor actor, const Dali::PanGesture& gesture);
	void onWheelEvent(const Dali::WheelEvent& event);
	void onCurrentPageChanged(int newPageIndex);
	bool onMenuTouched(Dali::Actor actor, const Dali::TouchData& touch);
	bool onEnableBluetoothButtonClicked(Dali::Toolkit::Button button);
	bool onLockDisplayButtonClicked(Dali::Toolkit::Button button);
	bool onMuteAudioButtonClicked(Dali::Toolkit::Button button);
	bool onInformationButtonClicked(Dali::Toolkit::Button button);
	bool onQuitButtonClicked(Dali::Toolkit::Button button);
	AltitudeAudio altitudeAudio;
	ClimbAudio climbAudio;
	AltitudeRing altitudeRing;
	ClimbRing climbRing;
	AltitudeLabel altitudeLabel;
	ClimbLabel climbLabel;
	SpeedLabel speedLabel;
	PageView pageView;
	AltitudePage altitudePage;
	ClimbPage climbPage;
	SpeedPage speedPage;
	Menu menu;
	time_t lastMenuTouch = 0;
	Dali::TapGestureDetector tapDetector;
	Dali::PanGestureDetector panDetector;
	Signal<> quitSignl;
	Signal<bool> lockDisplaySignl;
	Signal<bool> enableBluetoothSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_USERINTERFACE_H
