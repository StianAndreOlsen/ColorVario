#ifndef KYSTSOFT_CLIMBWRITER_H
#define KYSTSOFT_CLIMBWRITER_H

#include "ValueWriter.h"

namespace Kystsoft {

class ClimbWriter : public ValueWriter
{
public:
	ClimbWriter() { setTitle("Climb"); setUnit("m/s"); }
	void load(const Settings& settings) override { ValueWriter::load(settings, "ClimbLabel"); }
	void setUnit(const std::string& unit) override;
	double climb() const { return value(); }
	void setClimb(double climb) { setValue(climb); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBWRITER_H
