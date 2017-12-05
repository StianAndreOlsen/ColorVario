#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include "Averager.h"
#include "SensorListener.h"

namespace Kystsoft {

class Variometer
{
public:
	Variometer();
	Variometer(const Variometer& other) = delete;
	Variometer& operator=(const Variometer& rhs) = delete;
	bool isStarted() const { return pressureListener.isStarted(); }
	void start() { pressureListener.start(); }
	void stop() { pressureListener.stop(); }
	void toggleStartStop() { pressureListener.toggleStartStop(); }
	const Signal<float>& climbSignal() const { return climbSignl; }
	const Signal<float>& altitudeSignal() const { return altitudeSignl; }
private:
	void onPressureSensorEvent(Sensor sensor, sensor_event_s* event);
	SensorListener pressureListener;
	uint64_t lastTimestamp = 0;
	float lastAltitude = 0;
	Averager averageClimb;
	Averager averageAltitude;
	Signal<float> climbSignl;
	Signal<float> altitudeSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
