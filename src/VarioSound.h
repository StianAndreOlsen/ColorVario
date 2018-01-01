#ifndef KYSTSOFT_VARIOSOUND_H
#define KYSTSOFT_VARIOSOUND_H

#include "VarioSoundPoint.h"
#include "Settings.h"

namespace Kystsoft {

class VarioSound
{
public:
	VarioSound() {}
	VarioSound(const Settings& settings) { load(settings); }
	void load(const Settings& settings);
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
	float frequency(float climb) const;
	float period(float climb) const;
	float duty(float climb) const;
	static float defaultFrequency(float climb);
	static float defaultPeriod(float climb);
	static float defaultDuty(float climb);
private:
	size_t soundPointInterval(float climb) const;
	float climbSoundOn = 0.0f;
	float climbSoundOff = 0.0f;
	float sinkSoundOn = -3.0f;
	float sinkSoundOff = -3.0f;
	std::vector<VarioSoundPoint> soundPoints;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOSOUND_H
