#ifndef KYSTSOFT_VALUELABEL_H
#define KYSTSOFT_VALUELABEL_H

#include "Averager.h"
#include "Callback.h"
#include "Settings.h"
#include "TextLabel.h"
#include <sstream>

namespace Kystsoft {

class ValueLabel : public TextLabel
{
public:
	ValueLabel() { os.precision(15); }
	ValueLabel(const Dali::Toolkit::TextLabel& other) : TextLabel(other) {}
	ValueLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { TextLabel::operator=(rhs); return *this; }
	virtual ~ValueLabel() noexcept {}
	virtual void load(const Settings& settings) = 0;
	const std::string& unit() const { return unt; }
	virtual void setUnit(const std::string& unit) { unt = unit; }
	bool showUnit() const { return showUnt; }
	void setShowUnit(bool showUnit) { showUnt = showUnit; }
	void setSamplingInterval(double interval) { average.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { average.setAveragingInterval(interval); }
	void setRoundToNearest(double multiple) { mult = multiple; }
	void setValue(double value);
protected:
	void load(const Settings& settings, const std::string& section);
	using ConvertCallback = Callback<double, double*>;
	const ConvertCallback& convertCallback() const { return convertCb; }
private:
	std::string unt;
	bool showUnt = true;
	ConvertCallback convertCb;
	Averager<double> average;
	double mult = 0;
	std::ostringstream os;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUELABEL_H
