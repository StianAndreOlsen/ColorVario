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

class UserInterface : public Dali::ConnectionTracker
{
public:
	UserInterface();
	void create();
	void load(const Settings& settings);
	enum class Page
	{
		Quit,
		Altitude,
		Climb,
		Speed,
		Error
	};
	Page currentPage() const { return Page(curPage); }
	void setCurrentPage(Page page) { showPage(int(page)); }
	void showNextPage() { showPage(curPage + 1); }
	void showPreviousPage() { showPage(curPage - 1); }
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
	void setLocationEnabled(bool enabled);
	const Signal<>& quitSignal() const { return quitSignl; }
	const Signal<bool>& lockDisplaySignal() const { return lockDisplaySignl; }
	const Signal<bool>& enableBluetoothSignal() const { return enableBluetoothSignl; }
private:
	int pageCount() const { return int(pages.size()); }
	Dali::Toolkit::Control page(int i) { return pages[size_t(i)]; }
	Dali::Toolkit::Control page(Page p) { return pages[size_t(p)]; }
	Dali::Toolkit::Control quitPage() { return page(Page::Quit); }
	Dali::Toolkit::Control altitudePage() { return page(Page::Altitude); }
	Dali::Toolkit::Control climbPage() { return page(Page::Climb); }
	Dali::Toolkit::Control speedPage() { return page(Page::Speed); }
	Dali::Toolkit::Control errorPage() { return page(Page::Error); }
	void createQuitPageContents(Dali::Vector2 pageSize);
	void createAltitudePageContents(Dali::Vector2 pageSize);
	void createClimbPageContents(Dali::Vector2 pageSize);
	void createSpeedPageContents(Dali::Vector2 pageSize);
	void createErrorPageContents(Dali::Vector2 pageSize);
	void createUpperToolbar(Dali::Vector2 pageSize);
	void createLowerToolbar(Dali::Vector2 pageSize);
	void showPage(int newPage);
	void onTouch(const Dali::TouchData& touch);
	void onWheelEvent(const Dali::WheelEvent& event);
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
	int curPage = -1;
	std::vector<Dali::Toolkit::Control> pages;
	PushButton quitButton;
	TextLabel altitudePageAltitudeLabel;
	TextLabel climbPageAltitudeLabel;
	TextLabel climbPageClimbLabel;
	TextLabel speedPageAltitudeLabel;
	TextLabel speedPageClimbLabel;
	TextLabel speedPageSpeedLabel;
	TextLabel errorLabel;
	Dali::Toolkit::Control upperToolbar;
	PushButton muteAudioButton;
	PushButton lockDisplayButton;
	Dali::Toolkit::Control lowerToolbar;
	PushButton enableBluetoothButton;
	PushButton enableLocationButton;
	time_t lastUpperToolbarTouch = 0;
	time_t lastLowerToolbarTouch = 0;
	Signal<> quitSignl;
	Signal<bool> lockDisplaySignl;
	Signal<bool> enableBluetoothSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_USERINTERFACE_H
