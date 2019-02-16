#ifndef KYSTSOFT_ALTITUDEPAINTER_H
#define KYSTSOFT_ALTITUDEPAINTER_H

#include "ValuePainter.h"

namespace Kystsoft {

class AltitudePainter : public ValuePainter
{
public:
	AltitudePainter() {}
	void load(const Settings& settings) override { ValuePainter::load(settings, "AltitudeColor"); }
	double altitude() const { return value(); }
	void setAltitude(double altitude) { setValue(altitude); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEPAINTER_H
