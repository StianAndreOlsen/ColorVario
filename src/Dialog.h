#ifndef KYSTSOFT_DIALOG_H
#define KYSTSOFT_DIALOG_H

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
	void close() { hide(); closedSignl.emit(); }
	const Signal<>& closedSignal() const { return closedSignl; }
protected:
	bool onKeyEvent(Dali::Toolkit::Control control, const Dali::KeyEvent& event);
	Dali::Toolkit::Control control;
	Signal<> closedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DIALOG_H
