#ifndef KYSTSOFT_VALUEPAINTER_H
#define KYSTSOFT_VALUEPAINTER_H

#include "Averager.h"
#include "Settings.h"
#include "ValueColor.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class ValuePainter
{
public:
	ValuePainter() {}
	ValuePainter(const ValuePainter& other) = delete;
	ValuePainter& operator=(const ValuePainter& rhs) = delete;
	virtual ~ValuePainter() noexcept {}
	virtual void load(const Settings& settings) = 0;
	const ValueColor& valueColor() const { return color; }
	void setValueColor(const ValueColor& valueColor) { color = valueColor; }
	void setSamplingInterval(double interval) { averageValue.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	auto canvas() const { return control; }
	void setCanvas(Dali::Toolkit::Control canvas = Dali::Toolkit::Control());
	double value() const { return averageValue; }
	void setValue(double value);
	static Dali::Property::Map colorGradient(Color color);
	Dali::Property::Map colorGradient() const { return colorGradient(currentColor); }
protected:
	void load(const Settings& settings, const std::string& section);
private:
	ValueColor color;
	Dali::Toolkit::Control control;
	Averager<double> averageValue;
	Color currentColor;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEPAINTER_H
