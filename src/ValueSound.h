#ifndef KYSTSOFT_VALUESOUND_H
#define KYSTSOFT_VALUESOUND_H

#include "Settings.h"
#include "ValueSoundPoint.h"
#include "Waveform.h"

namespace Kystsoft {

class ValueSound
{
public:
	void load(const Settings& settings, const std::string& section);
	Waveform waveform() const { return toneWaveform; }
	void setWaveform(Waveform waveform) { toneWaveform = waveform; }
	double upperSoundOnThreshold() const { return upperSoundOn; }
	double upperSoundOffThreshold() const { return upperSoundOff; }
	double lowerSoundOnThreshold() const { return lowerSoundOn; }
	double lowerSoundOffThreshold() const { return lowerSoundOff; }
	void setUpperSoundOnThreshold(double threshold) { upperSoundOn = threshold; }
	void setUpperSoundOffThreshold(double threshold) { upperSoundOff = threshold; }
	void setLowerSoundOnThreshold(double threshold) { lowerSoundOn = threshold; }
	void setLowerSoundOffThreshold(double threshold) { lowerSoundOff = threshold; }
	void addSoundPoint(const ValueSoundPoint& point) { soundPoints.push_back(point); }
	void addSoundPoint(double value, double frequency, double period, double duty)
		{ addSoundPoint(ValueSoundPoint(value, frequency, period, duty)); }
	void clearSoundPoints() { soundPoints.clear(); }
	double frequency(double value) const;
	double period(double value) const;
	double duty(double value) const;
private:
	size_t soundPointInterval(double value) const;
	Waveform toneWaveform = Waveform::Sawtooth;
	double upperSoundOn = 0;
	double upperSoundOff = 0;
	double lowerSoundOn = 0;
	double lowerSoundOff = 0;
	std::vector<ValueSoundPoint> soundPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUESOUND_H
