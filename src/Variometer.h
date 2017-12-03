#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include "AudioOutput.h"
#include "Averager.h"
#include "SensorListener.h"
#include "VarioSound.h"

namespace Kystsoft {

class Variometer
{
public:
	Variometer();
	Variometer(const Variometer& other) = delete;
	Variometer& operator=(const Variometer& rhs) = delete;
	~Variometer() { stop(); } // TODO: Fix as noexcept
	bool isStarted() const { return pressureListener.isStarted(); }
	void start();
	void stop();
//	void toggleStartStop() { pressureListener.toggleStartStop(); } // TODO: If audio is removed, this is sufficient
	const Signal<float>& climbSignal() const { return climbSignl; }
	const Signal<float>& altitudeSignal() const { return altitudeSignl; }
private:
	void onPressureSensorEvent(sensor_event_s* event);
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	void turnSoundOn();
	void turnSoundOff();
	SensorListener pressureListener;
	VarioSound sound;
	Averager averageClimb;
	Averager averageAltitude;
	Signal<float> climbSignl;
	Signal<float> altitudeSignl;
	AudioOutput audioOutput;
	uint64_t lastTimestamp = 0;
	float lastAltitude = 0;
	float lastCyclePhase = 0;
	float lastTonePhase = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
