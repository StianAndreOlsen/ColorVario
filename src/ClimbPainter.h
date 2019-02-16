#ifndef KYSTSOFT_CLIMBPAINTER_H
#define KYSTSOFT_CLIMBPAINTER_H

#include "ValuePainter.h"

namespace Kystsoft {

class ClimbPainter : public ValuePainter
{
public:
	ClimbPainter() {}
	void load(const Settings& settings) override { ValuePainter::load(settings, "ClimbColor"); }
	double climb() const { return value(); }
	void setClimb(double climb) { setValue(climb); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBPAINTER_H
