#ifndef KYSTSOFT_ALTITUDEOFFSETWRITER_H
#define KYSTSOFT_ALTITUDEOFFSETWRITER_H

#include "AltitudeWriter.h"
#include "Stopwatch.h"

namespace Kystsoft {

class AltitudeOffsetWriter : public AltitudeWriter
{
public:
	AltitudeOffsetWriter();
	void setUnit(const std::string& unit) override;
	double offset() const { return value(); }
	void setOffset(double offset);
	void incrementOffset();
	void decrementOffset();
private:
	void updateDelta();
	double initialDelta = 1;
	double currentDelta = 1;
	int successiveCount = 0;
	Stopwatch deltaTimer;
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEOFFSETWRITER_H
