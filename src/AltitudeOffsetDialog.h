#ifndef KYSTSOFT_ALTITUDEOFFSETDIALOG_H
#define KYSTSOFT_ALTITUDEOFFSETDIALOG_H

#include "AltitudeOffsetWriter.h"
#include "AltitudeWriter.h"
#include "Dialog.h"

namespace Kystsoft {

class AltitudeOffsetDialog : public Dialog
{
public:
	AltitudeOffsetDialog() {}
	void create(const Dali::Vector2& size);
	void load(const Settings& settings);
	void reject() override;
	void close() override;
	void setOffset(double offset);
	void setAltitude(double altitude);
	auto offsetLabel() const { return offsetLabl; }
	auto altitudeLabel() const { return altitudeLabl; }
	void onWheelEvent(const Dali::WheelEvent& event);
	const auto& offsetChangedSignal() const { return offsetChangedSignl; }
private:
	void increment();
	void decrement();
	bool onIncrementButtonClicked(Dali::Toolkit::Button) { increment(); return true; }
	bool onDecrementButtonClicked(Dali::Toolkit::Button) { decrement(); return true; }
	void onLongPressDetected(Dali::Actor actor, const Dali::LongPressGesture& gesture);
	void onVisible(bool visible);
	bool onVisibleTimer();
	double initialOffset = 0;
	Dali::Timer visibleTimer;
	AltitudeOffsetWriter offsetWriter;
	AltitudeWriter altitudeWriter;
	TextLabel offsetLabl;
	TextLabel altitudeLabl;
	PushButton incrementButton;
	PushButton decrementButton;
	Dali::LongPressGestureDetector longPressDetector;
	Signal<double> offsetChangedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEOFFSETDIALOG_H
