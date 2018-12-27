#ifndef KYSTSOFT_CLIMBLABEL_H
#define KYSTSOFT_CLIMBLABEL_H

#include "ValueLabel.h"

namespace Kystsoft {

class ClimbLabel : public ValueLabel
{
public:
	ClimbLabel() { setUnit("m/s"); }
	ClimbLabel(const Dali::Toolkit::TextLabel& other) : ValueLabel(other) {}
	ClimbLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { ValueLabel::operator=(rhs); return *this; }
	void load(const Settings& settings) override { ValueLabel::load(settings, "ClimbLabel"); }
	std::string name() const override { return "Climb"; }
	void setUnit(const std::string& unit) override;
	void setClimb(double climb) { setValue(climb); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBLABEL_H
