#include "ValueWriter.h"
#include <algorithm>
#include <sstream>

Kystsoft::ValueWriter::ValueWriter()
{
	valueLateTimer = Dali::Timer::New(5000); // default period
	valueLateTimer.TickSignal().Connect(this, &ValueWriter::onValueLate);
}

void Kystsoft::ValueWriter::setSamplingInterval(double interval)
{
	averageValue.setSamplingInterval(interval);
	auto milliSec = static_cast<unsigned int>(5 * interval * 1000 + 0.5);
	milliSec = std::max(1000u, milliSec); // at least one second
	auto running = valueLateTimer.IsRunning();
	valueLateTimer.SetInterval(milliSec);
	if (!running)
		valueLateTimer.Stop();
}

void Kystsoft::ValueWriter::setPaper(TextLabel paper)
{
	if (label != paper)
	{
		write(title());
		label = paper;
		write(writtenText());
	}
}

void Kystsoft::ValueWriter::setValue(double value)
{
	averageValue += value;
	valueValid = true;
	write(writtenText());
	valueLateTimer.Start();
}

double Kystsoft::ValueWriter::convertedValue() const
{
	double val = value();
	convertCb.execute(val, &val);
	return val;
}

std::string Kystsoft::ValueWriter::writtenText() const
{
	if (!valueValid)
		return title();
	std::ostringstream os;
	os.precision(15);
	if (showSign)
		os << std::showpos;
	os << writtenValue();
	if (showUnit)
		os << ' ' << unt;
	return os.str();
}

void Kystsoft::ValueWriter::load(const Settings& settings, const std::string& section)
{
	setUnit(settings.value(section + ".unit"));
	setShowUnit(settings.value(section + ".showUnit", true));
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
//	setRoundToNearest(settings.value(section + ".roundToNearest", 1.0));
}

void Kystsoft::ValueWriter::write(const std::string& text)
{
	if (label)
		label.setText(text);
}

bool Kystsoft::ValueWriter::onValueLate()
{
	valueValid = false;
	write(title());
	return false;
}
