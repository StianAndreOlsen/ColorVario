#ifndef KYSTSOFT_VALUESOUND_H
#define KYSTSOFT_VALUESOUND_H

#include "Settings.h"
#include "ValueSoundPoint.h"
#include "ValueZones.h"
#include "Waveform.h"

namespace Kystsoft {

class ValueSound
{
public:
	void load(const Settings& settings, const std::string& section);
	Waveform waveform() const { return toneWaveform; }
	void setWaveform(Waveform waveform) { toneWaveform = waveform; }
	const ValueZones& valueZones() const { return zones; }
	void setValueZones(const ValueZones& valueZones) { zones = valueZones; }
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
	ValueZones zones;
	std::vector<ValueSoundPoint> soundPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUESOUND_H
