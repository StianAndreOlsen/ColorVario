#ifndef KYSTSOFT_VARIOSOUNDSETTINGS_H
#define KYSTSOFT_VARIOSOUNDSETTINGS_H

#include "VarioSoundPoint.h"
#include <cstddef>
#include <vector>

namespace Kystsoft {

class VarioSoundSettings
{
public:
	VarioSoundSettings();
	float climbSoundOnThreshold() const { return climbSoundOn; }
	float climbSoundOffThreshold() const { return climbSoundOff; }
	float sinkSoundOnThreshold() const { return sinkSoundOn; }
	float sinkSoundOffThreshold() const { return sinkSoundOff; }
	void setClimbSoundOnThreshold(float threshold) { climbSoundOn = threshold; }
	void setClimbSoundOffThreshold(float threshold) { climbSoundOff = threshold; }
	void setSinkSoundOnThreshold(float threshold) { sinkSoundOn = threshold; }
	void setSinkSoundOffThreshold(float threshold) { sinkSoundOff = threshold; }
	void addSoundPoint(const VarioSoundPoint& point) { soundPoints.push_back(point); }
	void addSoundPoint(float climb, float frequency, float period, float duty)
		{ addSoundPoint(VarioSoundPoint(climb, frequency, period, duty)); }
	void clearSoundPoints() { soundPoints.clear(); }
	void sortSoundPoints();
	float frequency(float climb) const;
	float period(float climb) const;
	float duty(float climb) const;
	float defaultFrequency(float climb) const;
	float defaultPeriod(float climb) const;
	float defaultDuty(float climb) const;
private:
	size_t soundPointInterval(float climb) const;
	float climbSoundOn = -0.5f;
	float climbSoundOff = -0.5f;
	float sinkSoundOn = -3.0f;
	float sinkSoundOff = -3.0f;
	std::vector<VarioSoundPoint> soundPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOSOUNDSETTINGS_H
