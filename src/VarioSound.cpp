#include "VarioSound.h"
#include <algorithm>

Kystsoft::VarioSound::VarioSound()
{
	// add the default sound points
	addSoundPoint(-10.00f,  201.4f, 0.847f, 1.00f);
	addSoundPoint( -8.92f,  202.7f, 0.844f, 1.00f);
	addSoundPoint( -7.83f,  205.5f, 0.839f, 1.00f);
	addSoundPoint( -6.75f,  211.1f, 0.831f, 1.00f);
	addSoundPoint( -5.67f,  222.1f, 0.816f, 1.00f);
	addSoundPoint( -4.58f,  243.5f, 0.790f, 1.00f);
	addSoundPoint( -3.50f,  283.9f, 0.748f, 1.00f);
	addSoundPoint( -3.49f,  284.4f, 0.748f, 0.90f);
	addSoundPoint( -2.67f,  335.1f, 0.700f, 0.90f);
	addSoundPoint( -1.83f,  409.7f, 0.636f, 0.90f);
	addSoundPoint( -1.00f,  508.7f, 0.557f, 0.90f);
	addSoundPoint( -0.99f,  510.0f, 0.556f, 0.05f);
	addSoundPoint(  0.00f,  650.0f, 0.450f, 0.06f);
	addSoundPoint(  0.01f,  651.7f, 0.449f, 0.50f);
	addSoundPoint(  1.00f,  817.9f, 0.343f, 0.50f);
	addSoundPoint(  2.00f,  980.6f, 0.250f, 0.50f);
	addSoundPoint(  3.00f, 1133.8f, 0.179f, 0.50f);
	addSoundPoint(  4.00f, 1273.9f, 0.130f, 0.50f);
	addSoundPoint(  5.00f, 1398.7f, 0.098f, 0.50f);
	addSoundPoint(  6.00f, 1507.5f, 0.078f, 0.50f);
	addSoundPoint(  7.00f, 1600.3f, 0.067f, 0.50f);
	addSoundPoint(  8.00f, 1678.2f, 0.060f, 0.50f);
	addSoundPoint(  9.00f, 1742.6f, 0.056f, 0.50f);
	addSoundPoint( 10.00f, 1795.2f, 0.053f, 0.50f);
}

void Kystsoft::VarioSound::sortSoundPoints()
{
	std::sort(soundPoints.begin(), soundPoints.end());
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
		return climb < -3.5f ? 1.0f : 0.9f;
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
