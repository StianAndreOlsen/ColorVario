#ifndef KYSTSOFT_VARIOSOUND_H
#define KYSTSOFT_VARIOSOUND_H

#include <cstddef>
#include <vector>

namespace Kystsoft {

class VarioSound
{
public:
	struct Point
	{
		float climb = 0; // vertical speed [m/s]
		float frequency = 0; // tone frequency [Hz]
		float period = 0; // total time (tone + silence) [s]
		float duty = 0; // tone time as a fraction of total time [-]
		Point() {}
		Point(float c, float f, float p, float d) : climb(c), frequency(f), period(p), duty(d) {}
		bool operator<(const Point& other) const { return climb < other.climb; }
	};
	VarioSound();
	void setClimbSoundOnThreshold(float threshold) { climbSoundOn = threshold; }
	void setClimbSoundOffThreshold(float threshold) { climbSoundOff = threshold; }
	void setSinkSoundOnThreshold(float threshold) { sinkSoundOn = threshold; }
	void setSinkSoundOffThreshold(float threshold) { sinkSoundOff = threshold; }
	void addPoint(const Point& point) { points.push_back(point); }
	void addPoint(float climb, float frequency, float period, float duty) { addPoint(Point(climb, frequency, period, duty)); }
	void sortPoints();
	void setClimb(float climb);
	float climb() const { return currentClimb; }
	float frequency() const;
	float period() const;
	float duty() const;
	float frequency(float climb) { setClimb(climb); return frequency(); }
	float period(float climb) { setClimb(climb); return period(); }
	float duty(float climb) { setClimb(climb); return duty(); }
	bool isSoundOn() const { return soundOn != 0; }
	bool isSoundOff() const { return soundOn == 0; }
	bool isClimbSoundOn() const { return soundOn > 0; }
	bool isSinkSoundOn() const { return soundOn < 0; }
private:
	float climbSoundOn = -0.5f;
	float climbSoundOff = -0.5f;
	float sinkSoundOn = -2.5f;
	float sinkSoundOff = -2.5f;
	std::vector<Point> points;
	float currentClimb = 0;
	short soundOn = 0;
	size_t i = 0; // current point interval
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOSOUND_H
