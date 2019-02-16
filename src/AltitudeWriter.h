#ifndef KYSTSOFT_ALTITUDEWRITER_H
#define KYSTSOFT_ALTITUDEWRITER_H

#include "ValueWriter.h"

namespace Kystsoft {

class AltitudeWriter : public ValueWriter
{
public:
	AltitudeWriter() { setTitle("Altitude"); setUnit("m"); }
	void load(const Settings& settings) override { ValueWriter::load(settings, "AltitudeLabel"); }
	void setUnit(const std::string& unit) override;
	double altitude() const { return value(); }
	void setAltitude(double altitude) { setValue(altitude); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEWRITER_H
