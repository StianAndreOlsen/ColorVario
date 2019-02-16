#include "AltitudeOffsetWriter.h"

Kystsoft::AltitudeOffsetWriter::AltitudeOffsetWriter()
{
	setTitle("Offset");
	setAlwaysShowSign(true);
	setSamplingInterval(60 * 60 * 24 * 365); // one year; the user can be really slow ;-)
	setValue(0); // validate the initial value
}

void Kystsoft::AltitudeOffsetWriter::setUnit(const std::string& unit)
{
	AltitudeWriter::setUnit(unit);
	if (AltitudeWriter::unit() == "ft")
		initialDelta = currentDelta = 2 * 0.3048;
	else
		initialDelta = currentDelta = 1;
}

void Kystsoft::AltitudeOffsetWriter::setOffset(double offset)
{
	setValue(offset);
	currentDelta = initialDelta;
	successiveCount = 0;
}

void Kystsoft::AltitudeOffsetWriter::incrementOffset()
{
	if (successiveCount < 0)
		successiveCount = 1;
	else
		++successiveCount;
	updateDelta();
	setValue(offset() + currentDelta);
}

void Kystsoft::AltitudeOffsetWriter::decrementOffset()
{
	if (successiveCount > 0)
		successiveCount = -1;
	else
		--successiveCount;
	updateDelta();
	setValue(offset() - currentDelta);
}

void Kystsoft::AltitudeOffsetWriter::updateDelta()
{
	auto timeElapsed = deltaTimer.seconds();
	if (timeElapsed >= 0.5)
		successiveCount = 0;
	if (timeElapsed > 2.5)
		currentDelta = initialDelta;
	else if (timeElapsed > 1.5 && currentDelta > 5 * initialDelta)
		currentDelta /= 10;
	else if (timeElapsed < 0.5 && currentDelta < 50 * initialDelta && std::abs(successiveCount) > 5)
	{
		auto x = offset();
		auto m = currentDelta;
		if (std::fabs(mround(x, 10 * m) - mround(x, m)) < m / 2)
		{
			currentDelta *= 10;
			successiveCount = 0;
		}
	}
	deltaTimer.start();
}
