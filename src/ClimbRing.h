#ifndef KYSTSOFT_CLIMBRING_H
#define KYSTSOFT_CLIMBRING_H

#include "ValueRing.h"

namespace Kystsoft {

class ClimbRing : public ValueRing
{
public:
	ClimbRing() {}
	ClimbRing(const Dali::Toolkit::Control& other) : ValueRing(other) {}
	ClimbRing& operator=(const Dali::Toolkit::Control& rhs) { ValueRing::operator=(rhs); return *this; }
	void load(const Settings& settings) override { ValueRing::load(settings, "ClimbColor"); }
	void setClimb(double climb) { setValue(climb); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBRING_H
