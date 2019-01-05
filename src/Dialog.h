#ifndef KYSTSOFT_DIALOG_H
#define KYSTSOFT_DIALOG_H

#include "PushButton.h"
#include "Signal.h"

namespace Kystsoft {

class Dialog : public Dali::ConnectionTracker
{
public:
	Dialog() {}
	Dialog(const Dialog& other) = delete;
	Dialog& operator=(const Dialog& rhs) = delete;
	virtual ~Dialog() noexcept {} // not really required since base class has a virtual destructor but Tizen Studio is whining
	operator Dali::Toolkit::Control() const { return control; }
	void create(const Dali::Vector2& size);
	bool isVisible() const { return control.IsVisible(); }
	virtual void setVisible(bool visible);
	void show() { setVisible(true); }
	void hide() { setVisible(false); }
	void accept() { acceptedSignl.emit(); close(); }
	void reject() { rejectedSignl.emit(); close(); }
	void close() { hide(); closedSignl.emit(); }
	const Signal<>& acceptedSignal() const { return acceptedSignl; }
	const Signal<>& rejectedSignal() const { return rejectedSignl; }
	const Signal<>& closedSignal() const { return closedSignl; }
protected:
	bool onKeyEvent(Dali::Toolkit::Control control, const Dali::KeyEvent& event);
	bool onAcceptButtonClicked(Dali::Toolkit::Button) { accept(); return true; }
	bool onRejectButtonClicked(Dali::Toolkit::Button) { reject(); return true; }
	bool onCloseButtonClicked(Dali::Toolkit::Button) { close(); return true; }
	Dali::Toolkit::Control control;
	PushButton acceptButton;
	PushButton rejectButton;
	PushButton closeButton;
	Signal<> acceptedSignl;
	Signal<> rejectedSignl;
	Signal<> closedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DIALOG_H
