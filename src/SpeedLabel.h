#ifndef KYSTSOFT_SPEEDLABEL_H
#define KYSTSOFT_SPEEDLABEL_H

#include "ValueLabel.h"

namespace Kystsoft {

class SpeedLabel : public ValueLabel
{
public:
	SpeedLabel() { setUnit("km/h"); }
	SpeedLabel(const Dali::Toolkit::TextLabel& other) : ValueLabel(other) {}
	SpeedLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { ValueLabel::operator=(rhs); return *this; }
	void load(const Settings& settings) override { ValueLabel::load(settings, "SpeedLabel"); }
	void setUnit(const std::string& unit) override;
	void setSpeed(double speed) { setValue(speed); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_SPEEDLABEL_H
