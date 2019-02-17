#ifndef KYSTSOFT_PUSHBUTTON_H
#define KYSTSOFT_PUSHBUTTON_H

#include "Color.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

// https://developer.tizen.org/development/guides/native-application/user-interface/dali/ui-components/buttons
// https://developer.tizen.org/development/guides/.net-application/natural-user-interface/ui-components/button
class PushButton : public Dali::Toolkit::PushButton
{
public:
	PushButton() {}
	PushButton(const Dali::Toolkit::PushButton& other) : Dali::Toolkit::PushButton(other) {}
	PushButton& operator=(const Dali::Toolkit::PushButton& rhs) { Dali::Toolkit::PushButton::operator=(rhs); return *this; }
	static PushButton New();
	static PushButton DownCast(BaseHandle handle) { return Dali::Toolkit::PushButton::DownCast(handle); }
	auto isDisabled() const { return GetProperty<bool>(Button::Property::DISABLED); }
	void setDisabled(bool disable) { SetProperty(Button::Property::DISABLED, disable); }
	void disable() { setDisabled(true); }
	auto isEnabled() const { return !isDisabled(); }
	void setEnabled(bool enable) { setDisabled(!enable); }
	void enable() { setEnabled(true); }
	auto isCheckable() const { return GetProperty<bool>(Button::Property::TOGGLABLE); }
	void setCheckable(bool checkable) { SetProperty(Button::Property::TOGGLABLE, checkable); }
	auto isChecked() const { return GetProperty<bool>(Button::Property::SELECTED); }
	void setChecked(bool checked) { SetProperty(Button::Property::SELECTED, checked); }
	void setEllipticBackground(const Color& color); // TODO: Consider removing!
	void setUnselectedImage(const std::string& file) { SetProperty(Button::Property::UNSELECTED_STATE_IMAGE, file); }
	void setSelectedImage(const std::string& file) { SetProperty(Button::Property::SELECTED_STATE_IMAGE, file); }
	void setDisabledImage(const std::string& file) { SetProperty(Button::Property::DISABLED_STATE_IMAGE, file); }
	auto label() const { return GetProperty<std::string>(Button::Property::LABEL); }
	void setLabel(const std::string& label) { SetProperty(Button::Property::LABEL, label); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_PUSHBUTTON_H
