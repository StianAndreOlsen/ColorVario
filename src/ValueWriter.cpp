#include "ValueWriter.h"
#include <sstream>

void Kystsoft::ValueWriter::setPaper(TextLabel paper)
{
	if (label != paper)
	{
		if (label)
			label.setText(title());
		label = paper;
		if (label)
			label.setText(writtenText());
	}
}

void Kystsoft::ValueWriter::setValue(double value)
{
	averageValue += value;
	if (label)
		label.setText(writtenText());
}

double Kystsoft::ValueWriter::convertedValue() const
{
	double val = value();
	convertCb.execute(val, &val);
	return val;
}

std::string Kystsoft::ValueWriter::writtenText() const
{
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
