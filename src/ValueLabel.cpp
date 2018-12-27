#include "ValueLabel.h"
#include "MathFunctions.h"
#include <sstream>

Kystsoft::ValueLabel& Kystsoft::ValueLabel::operator=(const Dali::Toolkit::TextLabel& rhs)
{
	if (*this != rhs)
	{
		if (*this)
			setText(name());
		TextLabel::operator=(rhs);
		if (*this)
			setText(label());
	}
	return *this;
}

std::string Kystsoft::ValueLabel::label() const
{
	std::ostringstream os;
	os.precision(15);
	os << mround(averageValue, multipl);
	if (showUnt)
		os << ' ' << unt;
	return os.str();
}

void Kystsoft::ValueLabel::setValue(double value)
{
	convertCb.execute(value, &value);
	averageValue += value;
	if (*this)
		setText(label());
}

void Kystsoft::ValueLabel::load(const Settings& settings, const std::string& section)
{
	setUnit(settings.value(section + ".unit"));
	setShowUnit(settings.value(section + ".showUnit", true));
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
//	setRoundToNearest(settings.value(section + ".roundToNearest", 1.0));
}
