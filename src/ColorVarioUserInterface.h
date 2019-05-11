#ifndef KYSTSOFT_COLORVARIO_USERINTERFACE_H
#define KYSTSOFT_COLORVARIO_USERINTERFACE_H

#include "AltitudeAudio.h"
#include "AltitudeOffsetDialog.h"
#include "AltitudePage.h"
#include "AltitudePainter.h"
#include "AltitudeWriter.h"
#include "ClimbAudio.h"
#include "ClimbPage.h"
#include "ClimbPainter.h"
#include "ClimbWriter.h"
#include "ColorVarioMenu.h"
#include "MessageDialog.h"
#include "PageView.h"
#include "Signal.h"
#include "SpeedPage.h"
#include "SpeedWriter.h"

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
	bool isGpsRequired() const { return pageView.currentPageIndex() == 2; }
	void setAltitudeSamplingInterval(double interval);
	void setClimbSamplingInterval(double interval);
	void setSpeedSamplingInterval(double interval);
	void setAltitudeAccuracy(double accuracy);
	void setAltitudeOffset(double offset);
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
	const auto& goBackSignal() const { return goBackSignl; }
	const auto& enableBluetoothSignal() const { return enableBluetoothSignl; }
	const auto& lockDisplaySignal() const { return lockDisplaySignl; }
	const auto& quitSignal() const { return quitSignl; }
	const auto& pageTapDetectedSignal() const { return pageTapDetectedSignl; }
	const auto& altitudeOffsetChangedSignal() const { return altitudeOffsetDialog.offsetChangedSignal(); }
private:
	void createPageLayer();
	void createMenuLayer();
	void createDialogLayer();
	void updateMessageButton();
	void onWheelEvent(const Dali::WheelEvent& event);
	void onGoBack() { goBackSignl.emit(); }
	void onCurrentPageChanged(int newPageIndex);
	void onMenuVisible(bool visible);
	bool onEnableBluetoothButtonClicked(Dali::Toolkit::Button button);
	bool onLockDisplayButtonClicked(Dali::Toolkit::Button button);
	bool onMuteAudioButtonClicked(Dali::Toolkit::Button button);
	bool onMessageButtonClicked(Dali::Toolkit::Button button);
	bool onQuitButtonClicked(Dali::Toolkit::Button button);
	bool onQuitTimer();
	void onTapDetected(Dali::Actor actor, const Dali::TapGesture& gesture);
	void onVerticalPanDetected(Dali::Actor actor, const Dali::PanGesture& gesture);
	void onLongPressDetected(Dali::Actor actor, const Dali::LongPressGesture& gesture);
	AltitudeAudio altitudeAudio;
	ClimbAudio climbAudio;
	AltitudePainter altitudePainter;
	ClimbPainter climbPainter;
	AltitudeWriter altitudeWriter;
	ClimbWriter climbWriter;
	SpeedWriter speedWriter;
	PageView pageView;
	AltitudePage altitudePage;
	ClimbPage climbPage;
	SpeedPage speedPage;
	Menu menu;
	MessageDialog messageDialog;
	AltitudeOffsetDialog altitudeOffsetDialog;
	Dali::Timer quitTimer;
	Dali::TapGestureDetector tapDetector;
	Dali::PanGestureDetector verticalPanDetector;
	Dali::LongPressGestureDetector longPressDetector;
	Signal<> goBackSignl;
	Signal<bool> enableBluetoothSignl;
	Signal<bool> lockDisplaySignl;
	Signal<> quitSignl;
	Signal<> pageTapDetectedSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_USERINTERFACE_H
