#include "ValueSound.h"

void Kystsoft::ValueSound::load(const Settings& settings, const std::string& section)
{
	// tone waveform
	std::string waveform = settings.value(section + ".waveform");
	if (waveform == "sine")
		toneWaveform = Waveform::Sine;
	else if (waveform == "square")
		toneWaveform = Waveform::Square;
	else if (waveform == "triangle")
		toneWaveform = Waveform::Triangle;
	else
		toneWaveform = Waveform::Sawtooth;

	// sound on and off zones
	zones.load(settings, section);

	// sound points
	clearSoundPoints();
	for (const auto& value : settings.values(section + ".point"))
	{
		ValueSoundPoint point;
		if (point.load(value))
			addSoundPoint(point);
	}
}

double Kystsoft::ValueSound::frequency(double value) const
{
	if (zones.isOff(value))
		return 0;
	if (soundPoints.empty())
		return 500;
	if (soundPoints.size() < 2)
		return soundPoints[0].frequency;
	if (value <= soundPoints.front().value)
		return soundPoints.front().frequency;
	if (value >= soundPoints.back().value)
		return soundPoints.back().frequency;
	size_t i = soundPointInterval(value);
	double x = value;
	double x1 = soundPoints[i].value;
	double x2 = soundPoints[i+1].value;
	double y1 = soundPoints[i].frequency;
	double y2 = soundPoints[i+1].frequency;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

double Kystsoft::ValueSound::period(double value) const
{
	if (zones.isOff(value))
		return 0;
	if (soundPoints.empty())
		return 1;
	if (soundPoints.size() < 2)
		return soundPoints[0].period;
	if (value <= soundPoints.front().value)
		return soundPoints.front().period;
	if (value >= soundPoints.back().value)
		return soundPoints.back().period;
	size_t i = soundPointInterval(value);
	double x = value;
	double x1 = soundPoints[i].value;
	double x2 = soundPoints[i+1].value;
	double y1 = soundPoints[i].period;
	double y2 = soundPoints[i+1].period;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

double Kystsoft::ValueSound::duty(double value) const
{
	if (zones.isOff(value))
		return 0;
	if (soundPoints.empty())
		return 1;
	if (soundPoints.size() < 2)
		return soundPoints[0].duty;
	if (value <= soundPoints.front().value)
		return soundPoints.front().duty;
	if (value >= soundPoints.back().value)
		return soundPoints.back().duty;
	size_t i = soundPointInterval(value);
	double x = value;
	double x1 = soundPoints[i].value;
	double x2 = soundPoints[i+1].value;
	double y1 = soundPoints[i].duty;
	double y2 = soundPoints[i+1].duty;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

size_t Kystsoft::ValueSound::soundPointInterval(double value) const
{
	if (soundPoints.size() > 2)
		for (size_t i = soundPoints.size() - 2; i > 0; --i)
			if (value >= soundPoints[i].value)
				return i;
	return 0;
}
