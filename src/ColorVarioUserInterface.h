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

// TODO: Figure out if it is ok to have two ConnectionTracker classes
class UserInterface : public Dali::ConnectionTracker
{
public:
	enum class Page
	{
		Quit,
		Altitude,
		Climb,
		Speed,
		Error
	};
	UserInterface();
	void create();
	void load(const Settings& settings);
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
	void onDisplayLocked(bool locked);
	void onBluetoothEnabled(bool enabled);
	void onBluetoothConnected(bool connected);
	const Signal<>& quitSignal() const { return quitSignl; }
	const Signal<bool>& lockDisplaySignal() const { return lockDisplaySignl; }
	const Signal<bool>& enableBluetoothSignal() const { return enableBluetoothSignl; }
private:
	Dali::Toolkit::Control page(int p) { return pages[size_t(p)]; }
	Dali::Toolkit::Control page(Page p) { return pages[size_t(p)]; }
	Dali::Toolkit::Control quitPage() { return page(Page::Quit); }
	Dali::Toolkit::Control altitudePage() { return page(Page::Altitude); }
	Dali::Toolkit::Control climbPage() { return page(Page::Climb); }
	Dali::Toolkit::Control speedPage() { return page(Page::Speed); }
	Dali::Toolkit::Control errorPage() { return page(Page::Error); }
	void createAltitudeRing(Dali::Vector2 pageSize);
	void createClimbRing(Dali::Vector2 pageSize);
	void createQuitPage(Dali::Vector2 pageSize);
	void createAltitudePageLabels(Dali::Vector2 pageSize);
	void createClimbPageLabels(Dali::Vector2 pageSize);
	void createSpeedPageLabels(Dali::Vector2 pageSize);
	void createUpperToolbar(Dali::Vector2 pageSize);
	void createLowerToolbar(Dali::Vector2 pageSize);
	void showPage(int newPage);
	void onAltitudeAudioMuted(bool muted);
	void onClimbAudioMuted(bool muted);
	void onTouch(const Dali::TouchData& touch);
	void onWheelEvent(const Dali::WheelEvent& event);
	bool onQuitButtonClicked(Dali::Toolkit::Button button);
	bool onMuteAudioButtonClicked(Dali::Toolkit::Button button);
	bool onLockDisplayButtonClicked(Dali::Toolkit::Button button);
	bool onEnableBluetoothButtonClicked(Dali::Toolkit::Button button);
	AltitudeAudio altitudeAudio;
	AltitudeLabel altitudeLabel;
	AltitudeRing altitudeRing;
	ClimbAudio climbAudio;
	ClimbLabel climbLabel;
	ClimbRing climbRing;
	SpeedLabel speedLabel;
	int curPage = 0;
	std::vector<Dali::Toolkit::Control> pages;
	PushButton quitButton;
	TextLabel altitudePageAltitudeLabel;
	TextLabel climbPageClimbLabel;
	TextLabel climbPageAltitudeLabel;
	TextLabel speedPageAltitudeLabel;
	TextLabel speedPageClimbLabel;
	TextLabel speedPageSpeedLabel;
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
