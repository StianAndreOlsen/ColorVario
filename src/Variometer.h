#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include "SampleAverager.h"
#include <dali-toolkit/dali-toolkit.h>
#include <audio_io.h>
#include <sensor.h>
#include "VarioSound.h"

namespace Kystsoft {

class Variometer
{
public:
	Variometer() : averageClimb(10), averageAltitude(10) {}
	~Variometer() { stop(); }
	int start();
	void stop();
	bool isStarted() const { return pressureListener != nullptr; }
	void setClimbLabel(Dali::Toolkit::TextLabel* label) { climbLabel = label; }
	void setAltitudeLabel(Dali::Toolkit::TextLabel* label) { altitudeLabel = label; }
	float currentAltitude() const { return lastAltitude; }
	void onPressureChange(unsigned long long timestamp, float pressure);
	void onAudioRequested();
	void onAudioRequested(size_t bytesRequested);
private:
	bool turnSoundOn();
	bool turnSoundOff();
	VarioSound sound;
	SampleAverager averageClimb;
	SampleAverager averageAltitude;
	Dali::Toolkit::TextLabel* climbLabel = nullptr;
	Dali::Toolkit::TextLabel* altitudeLabel = nullptr;
	sensor_listener_h pressureListener = nullptr;
	audio_out_h audioOutput = nullptr;
	size_t tonePointsPlayed = 0;
	size_t silencePointsPlayed = 0;
	unsigned long long lastTimestamp = 0;
	float lastAltitude = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
