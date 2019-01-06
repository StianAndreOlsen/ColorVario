#ifndef KYSTSOFT_ALTITUDEOFFSETDIALOG_H
#define KYSTSOFT_ALTITUDEOFFSETDIALOG_H

#include "AltitudeWriter.h"
#include "Dialog.h"
#include "Stopwatch.h"

namespace Kystsoft {

class AltitudeOffsetDialog : public Dialog
{
public:
	AltitudeOffsetDialog() {}
	void create(const Dali::Vector2& size);
	void reject() override;
	void close() override;
	void setOffset(double offset);
	void setAltitude(double altitude);
	auto offsetLabel() const { return TextLabel::DownCast(control.GetChildAt(1)); }
	auto altitudeLabel() const { return TextLabel::DownCast(control.GetChildAt(2)); }
	void onWheelEvent(const Dali::WheelEvent& event);
	const auto& offsetChangedSignal() const { return offsetChangedSignl; }
private:
	void increment();
	void decrement();
	bool onIncrementButtonClicked(Dali::Toolkit::Button) { increment(); return true; }
	bool onDecrementButtonClicked(Dali::Toolkit::Button) { decrement(); return true; }
	void onTapDetected(Dali::Actor actor, const Dali::TapGesture& gesture);
	void onVisible(bool visible);
	bool onVisibleTimer();
	double initialOffset = 0;
	double currentOffset = 0;
	double delta = 1;
	Dali::Timer visibleTimer;
	Stopwatch changeTimer;
	AltitudeWriter offsetWriter;
	AltitudeWriter altitudeWriter;
	PushButton incrementButton;
	PushButton decrementButton;
	Dali::TapGestureDetector tapDetector;
	Signal<double> offsetChangedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEOFFSETDIALOG_H
