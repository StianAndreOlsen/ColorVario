#ifndef KYSTSOFT_COLORVARIO_MENU_H
#define KYSTSOFT_COLORVARIO_MENU_H

#include "PushButton.h"
#include "Signal.h"

namespace Kystsoft {
namespace ColorVario {

class Menu : public Dali::ConnectionTracker
{
public:
	Menu() {}
	Menu(const Menu& other) = delete;
	Menu& operator=(const Menu& rhs) = delete;
	operator Dali::Toolkit::Control() const { return control; }
	void create(const Dali::Vector2& size);
	bool isVisible() const { return control.GetCurrentPosition().y > -control.GetTargetSize().height; }
	void setVisible(bool visible);
	void show() { setVisible(true); }
	void hide() { setVisible(false); }
	void showOrHide(float vy);
	void translate(float dy);
	const auto& visibleSignal() const { return visibleSignl; }
	auto buttonLayer() const { return Dali::Layer::DownCast(control.GetChildAt(1)); }
	auto enableBluetoothButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(0)); }
	auto lockDisplayButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(1)); }
	auto muteAudioButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(2)); }
	auto messageButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(3)); }
	auto quitButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(4)); }
	auto statusLayer() const { return Dali::Layer::DownCast(control.GetChildAt(2)); }
	auto locationIcon() const { return Dali::Toolkit::ImageView::DownCast(statusLayer().GetChildAt(0)); }
private:
	void createButtonLayer(const Dali::Vector2& menuSize, float menuRadius);
	void createStatusLayer(const Dali::Vector2& menuSize);
	bool onAutoHide() { hide(); return false; }
	void onStageEntered(Dali::Actor) { control.SetKeyInputFocus(); }
	bool onTouch(Dali::Actor, const Dali::TouchData&) { autoHideTimer.Start(); return true; }
	bool onKeyEvent(Dali::Toolkit::Control control, const Dali::KeyEvent& event);
	Dali::Toolkit::Control control;
	Dali::Timer autoHideTimer;
	Signal<bool> visibleSignl;
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_MENU_H
