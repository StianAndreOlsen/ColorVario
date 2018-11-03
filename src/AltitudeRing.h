#ifndef KYSTSOFT_ALTITUDERING_H
#define KYSTSOFT_ALTITUDERING_H

#include "ValueRing.h"

namespace Kystsoft {

class AltitudeRing : public ValueRing
{
public:
	AltitudeRing() {}
	AltitudeRing(const Dali::Toolkit::Control& other) : ValueRing(other) {}
	AltitudeRing& operator=(const Dali::Toolkit::Control& rhs) { ValueRing::operator=(rhs); return *this; }
	void load(const Settings& settings) override { ValueRing::load(settings, "AltitudeColor"); }
	void setAltitude(double altitude) { setValue(altitude); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDERING_H
