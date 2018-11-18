#ifndef KYSTSOFT_COLORVARIO_USERINTERFACE_H
#define KYSTSOFT_COLORVARIO_USERINTERFACE_H

#include "AltitudeAudio.h"
#include "AltitudeLabel.h"
#include "AltitudeRing.h"
#include "ClimbAudio.h"
#include "ClimbLabel.h"
#include "ClimbRing.h"
#include "ColorVarioPage.h"
#include "PushButton.h"
#include "Signal.h"
#include "SpeedLabel.h"
#include <ctime>
#include <vector>

namespace Kystsoft {
namespace ColorVario {

class UserInterface : public Dali::ConnectionTracker
{
public:
	void create();
	void load(const Settings& settings);
	int currentPageIndex() const { return curPageIndex; }
	Page::Type currentPageType() const { return Page::Type(curPageIndex); }
	void showPage(int pageIndex);
	void showPage(Page::Type pageType) { showPage(int(pageType)); }
	void showNextPage() { showPage(curPageIndex + 1); }
	void showPreviousPage() { showPage(curPageIndex - 1); }
	bool isMenuVisible() const { return menu.GetCurrentPosition().y > -menu.GetTargetSize().height; }
	void setMenuVisible(bool visible);
	void showMenu() { setMenuVisible(true); }
	void hideMenu() { setMenuVisible(false); }
	void setAltitudeSamplingInterval(double interval);
	void setClimbSamplingInterval(double interval);
	void setSpeedSamplingInterval(double interval);
	void setAltitudeTextColor(const Dali::Vector4& color);
	void setClimbTextColor(const Dali::Vector4& color);
	void setSpeedTextColor(const Dali::Vector4& color);
	void setAltitude(double altitude);
	void setClimb(double climb);
	void setSpeed(double speed);
	void setErrorMessage(const std::string& message);
	void setAudioMuted(bool muted);
	void setDisplayLocked(bool locked);
	void setBluetoothEnabled(bool enabled);
	void setBluetoothConnected(bool connected);
	void setLocationEnabled(bool enabled) { locationIcon.SetVisible(enabled); }
	const Signal<>& quitSignal() const { return quitSignl; }
	const Signal<bool>& lockDisplaySignal() const { return lockDisplaySignl; }
	const Signal<bool>& enableBluetoothSignal() const { return enableBluetoothSignl; }
private:
	Page page(Page::Type type) { return pages[size_t(type)]; }
	Page quitPage() { return page(Page::Type::Quit); }
	Page altitudePage() { return page(Page::Type::Altitude); }
	Page climbPage() { return page(Page::Type::Climb); }
	Page speedPage() { return page(Page::Type::Speed); }
	Page errorPage() { return page(Page::Type::Error); }
	PushButton quitButton() { return PushButton::DownCast(quitPage().GetChildAt(0)); }
	TextLabel altitudePageAltitudeLabel() { return TextLabel::DownCast(altitudePage().GetChildAt(0)); }
	TextLabel climbPageAltitudeLabel() { return TextLabel::DownCast(climbPage().GetChildAt(1)); }
	TextLabel climbPageClimbLabel() { return TextLabel::DownCast(climbPage().GetChildAt(0)); }
	TextLabel speedPageAltitudeLabel() { return TextLabel::DownCast(speedPage().GetChildAt(0)); }
	TextLabel speedPageClimbLabel() { return TextLabel::DownCast(speedPage().GetChildAt(1)); }
	TextLabel speedPageSpeedLabel() { return TextLabel::DownCast(speedPage().GetChildAt(2)); }
	TextLabel errorLabel() { return TextLabel::DownCast(errorPage().GetChildAt(0)); }
	void createMenu();
	void onTouch(const Dali::TouchData& touch);
	void onWheelEvent(const Dali::WheelEvent& event);
	bool onMenuTouched(Dali::Actor actor, const Dali::TouchData& touch);
	bool onQuitButtonClicked(Dali::Toolkit::Button button);
	bool onMuteAudioButtonClicked(Dali::Toolkit::Button button);
	bool onLockDisplayButtonClicked(Dali::Toolkit::Button button);
	bool onEnableBluetoothButtonClicked(Dali::Toolkit::Button button);
	AltitudeAudio altitudeAudio;
	ClimbAudio climbAudio;
	AltitudeRing altitudeRing;
	ClimbRing climbRing;
	AltitudeLabel altitudeLabel;
	ClimbLabel climbLabel;
	SpeedLabel speedLabel;
	int curPageIndex = int(Page::Type::Climb);
	std::vector<Page> pages;
	Dali::Toolkit::Control pageStrip;
	Dali::Toolkit::Control menu;
	PushButton muteAudioButton;
	PushButton lockDisplayButton;
	PushButton enableBluetoothButton;
	Dali::Toolkit::ImageView locationIcon;
	time_t lastMenuTouch = 0;
	Signal<> quitSignl;
	Signal<bool> lockDisplaySignl;
	Signal<bool> enableBluetoothSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_USERINTERFACE_H
