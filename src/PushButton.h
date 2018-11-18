#ifndef KYSTSOFT_PUSHBUTTON_H
#define KYSTSOFT_PUSHBUTTON_H

#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class PushButton : public Dali::Toolkit::PushButton
{
public:
	PushButton() {}
	PushButton(const Dali::Toolkit::PushButton& other) : Dali::Toolkit::PushButton(other) {}
	PushButton& operator=(const Dali::Toolkit::PushButton& rhs) { Dali::Toolkit::PushButton::operator=(rhs); return *this; }
	static PushButton DownCast(BaseHandle handle) { return Dali::Toolkit::PushButton::DownCast(handle); }
	bool isDisabled() const { return GetProperty<bool>(Button::Property::DISABLED); }
	void setDisabled(bool disable) { SetProperty(Button::Property::DISABLED, disable); }
	void disable() { setDisabled(true); }
	bool isEnabled() const { return !isDisabled(); }
	void setEnabled(bool enable) { setDisabled(!enable); }
	void enable() { setEnabled(true); }
	bool isCheckable() const { return GetProperty<bool>(Button::Property::TOGGLABLE); }
	void setCheckable(bool checkable) { SetProperty(Button::Property::TOGGLABLE, checkable); }
	bool isChecked() const { return GetProperty<bool>(Button::Property::SELECTED); }
	void setChecked(bool checked) { SetProperty(Button::Property::SELECTED, checked); }
	void setUnselectedImage(const std::string& file) { SetProperty(Button::Property::UNSELECTED_STATE_IMAGE, file); }
	void setSelectedImage(const std::string& file) { SetProperty(Button::Property::SELECTED_STATE_IMAGE, file); }
	void setDisabledImage(const std::string& file) { SetProperty(Button::Property::DISABLED_STATE_IMAGE, file); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_PUSHBUTTON_H
