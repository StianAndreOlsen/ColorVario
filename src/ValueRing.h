#ifndef KYSTSOFT_VALUERING_H
#define KYSTSOFT_VALUERING_H

#include "Averager.h"
#include "Settings.h"
#include "ValueColor.h"
#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class ValueRing : public Dali::Toolkit::Control
{
public:
	ValueRing() {}
	ValueRing(const Dali::Toolkit::Control& other) : Dali::Toolkit::Control(other) {}
	ValueRing& operator=(const Dali::Toolkit::Control& rhs) { Dali::Toolkit::Control::operator=(rhs); return *this; }
	virtual ~ValueRing() noexcept {}
	virtual void load(const Settings& settings) = 0;
	const ValueColor& valueColor() const { return color; }
	void setValueColor(const ValueColor& valueColor) { color = valueColor; }
	void setSamplingInterval(double interval) { averageValue.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	void setValue(double value);
protected:
	void load(const Settings& settings, const std::string& section);
private:
	Averager<double> averageValue;
	ValueColor color;
	Color currentColor = Dali::Color::TRANSPARENT;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUERING_H
