#include "ValueLabel.h"
#include "MathFunctions.h"

void Kystsoft::ValueLabel::setValue(double value)
{
	convertCb.execute(value, &value);
	averageValue += value;
	os.str("");
	os << mround(averageValue, multipl);
	if (showUnt)
		os << ' ' << unt;
	setText(os.str());
}

void Kystsoft::ValueLabel::load(const Settings& settings, const std::string& section)
{
	setUnit(settings.value(section + ".unit"));
	setShowUnit(settings.value(section + ".showUnit", true));
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
//	setRoundToNearest(settings.value(section + ".roundToNearest", 1.0));
}
