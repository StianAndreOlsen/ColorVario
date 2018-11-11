#ifndef KYSTSOFT_COLORVARIO_USERINTERFACE_H
#define KYSTSOFT_COLORVARIO_USERINTERFACE_H

#include "AltitudeAudio.h"
#include "AltitudeLabel.h"
#include "AltitudeRing.h"
#include "ClimbAudio.h"
#include "ClimbLabel.h"
#include "ClimbRing.h"
#include "PushButton.h"
#include "Signal.h"
#include "SpeedLabel.h"
#include <ctime>
#include <vector>

namespace Kystsoft {
namespace ColorVario {

// Note: The pages can be re-arranged just by changing the order in the below enum
enum class Page : int
{
	Quit,
	Altitude,
	Climb,
	Speed,
	Error
};
constexpr int pageCount = 5;
inline Page operator+(Page p, int i) { return Page(std::clamp(int(p) + 1, 0, pageCount - 1)); }
inline Page operator-(Page p, int i) { return Page(std::clamp(int(p) - 1, 0, pageCount - 1)); }

class UserInterface : public Dali::ConnectionTracker
{
public:
	void create();
	void load(const Settings& settings);
	Page currentPage() const { return curPage; }
	void showPage(Page page);
	void showNextPage() { showPage(curPage + 1); }
	void showPreviousPage() { showPage(curPage - 1); }
	void showMenu(bool show);
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
	Dali::Toolkit::Control page(Page p) { return pages[size_t(p)]; }
	Dali::Toolkit::Control quitPage() { return page(Page::Quit); }
	Dali::Toolkit::Control altitudePage() { return page(Page::Altitude); }
	Dali::Toolkit::Control climbPage() { return page(Page::Climb); }
	Dali::Toolkit::Control speedPage() { return page(Page::Speed); }
	Dali::Toolkit::Control errorPage() { return page(Page::Error); }
	void createQuitPageContents(const Dali::Vector2& pageSize);
	void createAltitudePageContents(const Dali::Vector2& pageSize);
	void createClimbPageContents(const Dali::Vector2& pageSize);
	void createSpeedPageContents(const Dali::Vector2& pageSize);
	void createErrorPageContents(const Dali::Vector2& pageSize);
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
	Page curPage = Page::Climb;
	std::vector<Dali::Toolkit::Control> pages;
	Dali::Toolkit::Control pageStrip;
	PushButton quitButton;
	TextLabel altitudePageAltitudeLabel;
	TextLabel climbPageAltitudeLabel;
	TextLabel climbPageClimbLabel;
	TextLabel speedPageAltitudeLabel;
	TextLabel speedPageClimbLabel;
	TextLabel speedPageSpeedLabel;
	TextLabel errorLabel;
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
