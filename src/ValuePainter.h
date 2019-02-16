#ifndef KYSTSOFT_VALUEPAINTER_H
#define KYSTSOFT_VALUEPAINTER_H

#include "Averager.h"
#include "Settings.h"
#include "ValueColor.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class ValuePainter : public Dali::ConnectionTracker
{
public:
	ValuePainter();
	ValuePainter(const ValuePainter& other) = delete;
	ValuePainter& operator=(const ValuePainter& rhs) = delete;
	virtual ~ValuePainter() noexcept {}
	virtual void load(const Settings& settings) = 0;
	void setSamplingInterval(double interval);
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	auto canvas() const { return control; }
	void setCanvas(Dali::Toolkit::Control canvas = Dali::Toolkit::Control());
	double value() const { return averageValue; }
	void setValue(double value);
	const auto& valueColor() const { return color; }
	void setValueColor(const ValueColor& valueColor) { color = valueColor; }
protected:
	void load(const Settings& settings, const std::string& section);
	static Dali::Property::Map colorGradient(Color color);
	void paint(Color color);
private:
	bool onValueLate();
	Dali::Toolkit::Control control;
	Averager<double> averageValue;
	ValueColor color;
	Color currentColor;
	Dali::Timer valueLateTimer;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEPAINTER_H
