#include "VarioSound.h"

void Kystsoft::VarioSound::load(const Settings& settings)
{
	// tone waveform
	std::string waveform = settings.value("Sound.waveform");
	if (waveform == "sine")
		toneWaveform = Waveform::Sine;
	else if (waveform == "square")
		toneWaveform = Waveform::Square;
	else if (waveform == "triangle")
		toneWaveform = Waveform::Triangle;
	else
		toneWaveform = Waveform::Sawtooth;

	// thresholds
	climbSoundOn = settings.value("Sound.climbSoundOn", climbSoundOn);
	climbSoundOff = settings.value("Sound.climbSoundOff", climbSoundOff);
	sinkSoundOn = settings.value("Sound.sinkSoundOn", sinkSoundOn);
	sinkSoundOff = settings.value("Sound.sinkSoundOff", sinkSoundOff);

	// sound points
	clearSoundPoints();
	for (auto value : settings.values("Sound.point"))
	{
		VarioSoundPoint point;
		std::istringstream is(value);
		if (point.load(is))
			addSoundPoint(point);
	}
}

float Kystsoft::VarioSound::frequency(float climb) const
{
	if (soundPoints.empty())
		return defaultFrequency(climb);
	if (soundPoints.size() < 2)
		return soundPoints[0].frequency;
	size_t i = soundPointInterval(climb);
	float x = climb;
	float x1 = soundPoints[i].climb;
	float x2 = soundPoints[i+1].climb;
	float y1 = soundPoints[i].frequency;
	float y2 = soundPoints[i+1].frequency;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

float Kystsoft::VarioSound::period(float climb) const
{
	if (soundPoints.empty())
		return defaultPeriod(climb);
	if (soundPoints.size() < 2)
		return soundPoints[0].period;
	size_t i = soundPointInterval(climb);
	float x = climb;
	float x1 = soundPoints[i].climb;
	float x2 = soundPoints[i+1].climb;
	float y1 = soundPoints[i].period;
	float y2 = soundPoints[i+1].period;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

float Kystsoft::VarioSound::duty(float climb) const
{
	if (soundPoints.empty())
		return defaultDuty(climb);
	if (soundPoints.size() < 2)
		return soundPoints[0].duty;
	size_t i = soundPointInterval(climb);
	float x = climb;
	float x1 = soundPoints[i].climb;
	float x2 = soundPoints[i+1].climb;
	float y1 = soundPoints[i].duty;
	float y2 = soundPoints[i+1].duty;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

float Kystsoft::VarioSound::defaultFrequency(float climb)
{
	if (climb < 0)
		return std::max(400 + climb * (400 - 200) / 10, 200.0f);
	return std::min(600 + climb * (1800 - 600) / 10, 1800.0f);
}

float Kystsoft::VarioSound::defaultPeriod(float climb)
{
	if (climb < 0)
		return std::min(0.5f + climb * (0.5f - 1.0f) / 10, 1.0f);
	return std::max(0.5f + climb * (0.05f - 0.5f) / 10, 0.05f);
}

float Kystsoft::VarioSound::defaultDuty(float climb)
{
	if (climb < -1)
		return climb < -4.0f ? 1.0f : 0.9f;
	return climb > 0 ? 0.5f : 0.05f;
}

size_t Kystsoft::VarioSound::soundPointInterval(float climb) const
{
	if (soundPoints.size() > 2)
	{
		size_t i = 0;
		for (; i < soundPoints.size() - 2; ++i)
			if (climb <= soundPoints[i+1].climb)
				return i;
		return i;
	}
	return 0;
}
