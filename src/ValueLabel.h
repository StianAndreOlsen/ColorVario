#ifndef KYSTSOFT_VALUELABEL_H
#define KYSTSOFT_VALUELABEL_H

#include "Averager.h"
#include "Callback.h"
#include "Settings.h"
#include "TextLabel.h"

namespace Kystsoft {

class ValueLabel : public TextLabel
{
public:
	ValueLabel() {}
	ValueLabel(const Dali::Toolkit::TextLabel& other) : TextLabel(other) {}
	ValueLabel& operator=(const Dali::Toolkit::TextLabel& rhs);
	virtual ~ValueLabel() noexcept {}
	virtual void load(const Settings& settings) = 0;
	virtual std::string name() const = 0;
	std::string label() const;
	const std::string& unit() const { return unt; }
	virtual void setUnit(const std::string& unit) { unt = unit; }
	bool showUnit() const { return showUnt; }
	void setShowUnit(bool showUnit) { showUnt = showUnit; }
	void setSamplingInterval(double interval) { averageValue.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	void setRoundToNearest(double multiple) { multipl = multiple; }
	void setValue(double value);
protected:
	void load(const Settings& settings, const std::string& section);
	using ConvertCallback = Callback<double, double*>;
	const ConvertCallback& convertCallback() const { return convertCb; }
private:
	std::string unt;
	bool showUnt = true;
	ConvertCallback convertCb;
	Averager<double> averageValue;
	double multipl = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUELABEL_H
