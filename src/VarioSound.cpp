#include <algorithm>
#include "VarioSound.h"

Kystsoft::VarioSound::VarioSound()
{
	// add the default sound points
	addPoint(-10.00f,  201.4f, 0.847f, 1.00f);
	addPoint( -8.92f,  202.7f, 0.844f, 1.00f);
	addPoint( -7.83f,  205.5f, 0.839f, 1.00f);
	addPoint( -6.75f,  211.1f, 0.831f, 1.00f);
	addPoint( -5.67f,  222.1f, 0.816f, 1.00f);
	addPoint( -4.58f,  243.5f, 0.790f, 1.00f);
	addPoint( -3.50f,  283.9f, 0.748f, 1.00f);
	addPoint( -3.49f,  284.4f, 0.748f, 0.90f);
	addPoint( -2.67f,  335.1f, 0.700f, 0.90f);
	addPoint( -1.83f,  409.7f, 0.636f, 0.90f);
	addPoint( -1.00f,  508.7f, 0.557f, 0.90f);
	addPoint( -0.99f,  510.0f, 0.556f, 0.05f);
	addPoint(  0.00f,  650.0f, 0.450f, 0.06f);
	addPoint(  0.01f,  651.7f, 0.449f, 0.50f);
	addPoint(  1.00f,  817.9f, 0.343f, 0.50f);
	addPoint(  2.00f,  980.6f, 0.250f, 0.50f);
	addPoint(  3.00f, 1133.8f, 0.179f, 0.50f);
	addPoint(  4.00f, 1273.9f, 0.130f, 0.50f);
	addPoint(  5.00f, 1398.7f, 0.098f, 0.50f);
	addPoint(  6.00f, 1507.5f, 0.078f, 0.50f);
	addPoint(  7.00f, 1600.3f, 0.067f, 0.50f);
	addPoint(  8.00f, 1678.2f, 0.060f, 0.50f);
	addPoint(  9.00f, 1742.6f, 0.056f, 0.50f);
	addPoint( 10.00f, 1795.2f, 0.053f, 0.50f);
}

void Kystsoft::VarioSound::sortPoints()
{
	std::sort(points.begin(), points.end());
}

void Kystsoft::VarioSound::setClimb(float climb)
{
	if (currentClimb == climb)
		return;
	float previousClimb = currentClimb;
	currentClimb = climb;

	// sound on or off
	if (currentClimb > previousClimb)
	{
		if (isSinkSoundOn() && currentClimb >= sinkSoundOff)
			soundOn = 0; // turn off sink sound
		if (isSoundOff() && currentClimb >= climbSoundOn)
			soundOn = 1; // turn on climb sound
	}
	else // remember that equality is tested at function start
	{
		if (isClimbSoundOn() && currentClimb <= climbSoundOff)
			soundOn = 0; // turn off climb sound
		if (isSoundOff() && currentClimb <= sinkSoundOn)
			soundOn = -1; // turn on sink sound
	}

	// find current point interval
	for (i = 0; i < points.size() - 2; ++i)
		if (currentClimb <= points[i+1].climb)
			break;
}

float Kystsoft::VarioSound::frequency() const
{
	if (points.empty())
		return 0;
	if (points.size() < 2)
		return points[0].frequency;
	float x1 = points[i].climb;
	float y1 = points[i].frequency;
	float x2 = points[i+1].climb;
	float y2 = points[i+1].frequency;
	float x = currentClimb;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

float Kystsoft::VarioSound::period() const
{
	if (points.empty())
		return 0;
	if (points.size() < 2)
		return points[0].period;
	float x1 = points[i].climb;
	float y1 = points[i].period;
	float x2 = points[i+1].climb;
	float y2 = points[i+1].period;
	float x = currentClimb;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}

float Kystsoft::VarioSound::duty() const
{
	if (points.empty())
		return 0;
	if (points.size() < 2)
		return points[0].duty;
	float x1 = points[i].climb;
	float y1 = points[i].duty;
	float x2 = points[i+1].climb;
	float y2 = points[i+1].duty;
	float x = currentClimb;
	return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
}
