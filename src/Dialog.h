#ifndef KYSTSOFT_DIALOG_H
#define KYSTSOFT_DIALOG_H

#include "PushButton.h"
#include "Signal.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class Dialog : public Dali::ConnectionTracker
{
public:
	Dialog() {}
	Dialog(const Dialog& other) = delete;
	Dialog& operator=(const Dialog& rhs) = delete;
	operator Dali::Toolkit::Control() const { return control; }
	void create(const Dali::Vector2& size);
	bool isVisible() const { return control.IsVisible(); }
	void setVisible(bool visible);
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
	bool onAcceptButtonClicked(Dali::Toolkit::Button button);
	bool onRejectButtonClicked(Dali::Toolkit::Button button);
	bool onCloseButtonClicked(Dali::Toolkit::Button button);
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
