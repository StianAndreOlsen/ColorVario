#ifndef KYSTSOFT_VALUEWRITER_H
#define KYSTSOFT_VALUEWRITER_H

#include "Averager.h"
#include "Callback.h"
#include "MathFunctions.h"
#include "Settings.h"
#include "TextLabel.h"

namespace Kystsoft {

class ValueWriter : public Dali::ConnectionTracker
{
public:
	ValueWriter();
	ValueWriter(const ValueWriter& other) = delete;
	ValueWriter& operator=(const ValueWriter& rhs) = delete;
	virtual ~ValueWriter() noexcept {}
	virtual void load(const Settings& settings) = 0;
	const auto& title() const { return titl; }
	void setTitle(const std::string& title) { titl = title; }
	const auto& unit() const { return unt; }
	virtual void setUnit(const std::string& unit) { unt = unit; }
	bool isAlwaysShowingSign() const { return showSign; }
	void setAlwaysShowSign(bool show) { showSign = show; }
	bool isShowingUnit() const { return showUnit; }
	void setShowUnit(bool show) { showUnit = show; }
	void setRoundToNearest(double multiple) { multipl = multiple; }
	void setSamplingInterval(double interval);
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	auto paper() const { return label; }
	void setPaper(TextLabel paper = TextLabel());
	double value() const { return averageValue; }
	void setValue(double value);
	double convertedValue() const;
	double writtenValue() const { return mround(convertedValue(), multipl); }
	std::string writtenText() const;
protected:
	void load(const Settings& settings, const std::string& section);
	const auto& convertCallback() const { return convertCb; }
	void write(const std::string& text);
private:
	bool onValueLate();
	TextLabel label;
	std::string titl;
	std::string unt;
	bool showSign = false;
	bool showUnit = true;
	double multipl = 0;
	Averager<double> averageValue;
	bool valueValid = false;
	Dali::Timer valueLateTimer;
	Callback<double, double*> convertCb;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEWRITER_H
