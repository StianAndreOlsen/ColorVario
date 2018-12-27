#ifndef KYSTSOFT_ALTITUDELABEL_H
#define KYSTSOFT_ALTITUDELABEL_H

#include "ValueLabel.h"

namespace Kystsoft {

class AltitudeLabel : public ValueLabel
{
public:
	AltitudeLabel() { setUnit("m"); }
	AltitudeLabel(const Dali::Toolkit::TextLabel& other) : ValueLabel(other) {}
	AltitudeLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { ValueLabel::operator=(rhs); return *this; }
	void load(const Settings& settings) override { ValueLabel::load(settings, "AltitudeLabel"); }
	std::string name() const override { return "Altitude"; }
	void setUnit(const std::string& unit) override;
	void setAltitude(double altitude) { setValue(altitude); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDELABEL_H
