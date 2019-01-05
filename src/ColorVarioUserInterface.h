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
#include "MessageDialog.h"
#include "PageView.h"
#include "Signal.h"
#include "SpeedLabel.h"
#include "SpeedPage.h"

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
	void setAltitudeSamplingInterval(double interval);
	void setClimbSamplingInterval(double interval);
	void setSpeedSamplingInterval(double interval);
	void setAltitudeAccuracy(double accuracy);
	void setAltitude(double altitude);
	void setClimb(double climb);
	void setSpeed(double speed);
	void setAudioMuted(bool muted);
	void setDisplayLocked(bool locked);
	void setBluetoothEnabled(bool enabled);
	void setBluetoothConnected(bool connected);
	void setLocationEnabled(bool enabled) { menu.locationIcon().SetVisible(enabled); }
	bool addMessage(const Message& message);
	bool removeMessage(const Message& message);
	const Signal<>& goBackSignal() const { return goBackSignl; }
	const Signal<bool>& enableBluetoothSignal() const { return enableBluetoothSignl; }
	const Signal<bool>& lockDisplaySignal() const { return lockDisplaySignl; }
	const Signal<>& quitSignal() const { return quitSignl; }
	const Signal<>& pageTappedSignal() const { return pageTappedSignl; }
private:
	void createPageLayer();
	void createMenuLayer();
	void createDialogLayer();
	void onTapGesture(Dali::Actor actor, const Dali::TapGesture& gesture);
	void onPanGesture(Dali::Actor actor, const Dali::PanGesture& gesture);
	void onWheelEvent(const Dali::WheelEvent& event);
	void onGoBack() { goBackSignl.emit(); }
	void onCurrentPageChanged(int newPageIndex);
	bool onEnableBluetoothButtonClicked(Dali::Toolkit::Button button);
	bool onLockDisplayButtonClicked(Dali::Toolkit::Button button);
	bool onMuteAudioButtonClicked(Dali::Toolkit::Button button);
	bool onMessageButtonClicked(Dali::Toolkit::Button button);
	bool onQuitButtonClicked(Dali::Toolkit::Button button);
	void updateMessageButton();
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
	Dali::TapGestureDetector tapDetector;
	Dali::PanGestureDetector panDetector;
	MessageDialog messageDialog;
	Signal<> goBackSignl;
	Signal<bool> enableBluetoothSignl;
	Signal<bool> lockDisplaySignl;
	Signal<> quitSignl;
	Signal<> pageTappedSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_USERINTERFACE_H
