#ifndef KYSTSOFT_SPEEDWRITER_H
#define KYSTSOFT_SPEEDWRITER_H

#include "ValueWriter.h"

namespace Kystsoft {

class SpeedWriter : public ValueWriter
{
public:
	SpeedWriter() { setTitle("Speed"); setUnit("km/h"); }
	void load(const Settings& settings) override { ValueWriter::load(settings, "SpeedLabel"); }
	void setUnit(const std::string& unit) override;
	double speed() const { return value(); }
	void setSpeed(double speed) { setValue(speed); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_SPEEDWRITER_H
