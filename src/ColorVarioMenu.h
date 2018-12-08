#ifndef KYSTSOFT_COLORVARIO_MENU_H
#define KYSTSOFT_COLORVARIO_MENU_H

#include "PushButton.h"

namespace Kystsoft {
namespace ColorVario {

class Menu : public Dali::Toolkit::Control
{
public:
	Menu() {}
	Menu(const Menu& other) : Dali::Toolkit::Control(other) {}
	Menu& operator=(const Menu& rhs) { Dali::Toolkit::Control::operator=(rhs); return *this; }
	static Menu New();
	auto buttonLayer() const { return Dali::Layer::DownCast(GetChildAt(1)); }
	auto enableBluetoothButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(0)); }
	auto lockDisplayButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(1)); }
	auto muteAudioButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(2)); }
	auto informationButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(3)); }
	auto quitButton() const { return PushButton::DownCast(buttonLayer().GetChildAt(4)); }
	auto statusLayer() const { return Dali::Layer::DownCast(GetChildAt(2)); }
	auto locationIcon() const { return Dali::Toolkit::ImageView::DownCast(statusLayer().GetChildAt(0)); }
private:
	Menu(const Dali::Toolkit::Control& other) : Dali::Toolkit::Control(other) {}
	static void createButtonLayer(Dali::Actor parent, const Dali::Vector2& menuSize, float menuRadius);
	static void createStatusLayer(Dali::Actor parent, const Dali::Vector2& menuSize);
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_MENU_H
