#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include "Averager.h"
#include "SensorListener.h"
#include "Settings.h"

namespace Kystsoft {

class Variometer
{
public:
	Variometer();
	Variometer(const Variometer& other) = delete;
	Variometer& operator=(const Variometer& rhs) = delete;
	void load(const Settings& settings);
	float samplingInterval() const { return interval_ms / 1000.0f; }
	void setSamplingInterval(float interval);
	float averagingInterval() const { return samplingInterval() * averageClimb.size(); }
	void setAveragingInterval(float interval);
	bool isStarted() const { return pressureListener.isStarted(); }
	void setStarted(bool started) { pressureListener.setStarted(started); }
	void start() { pressureListener.start(); }
	void stop() { pressureListener.stop(); }
	void toggleStartStop() { pressureListener.toggleStartStop(); }
	void setCurrentAltitude(float altitude) { currentAltitude = altitude; } // calibrates the altimeter
	const Signal<float>& climbSignal() const { return climbSignl; }
	const Signal<float>& altitudeSignal() const { return altitudeSignl; }
private:
	void onPressureSensorEvent(Sensor sensor, sensor_event_s* event);
	SensorListener pressureListener;
	uint32_t interval_ms = 100;
	float currentAltitude = -6.5e+6f; // smaller than the radius of earth
	float referencePressure = 1013.25f; // https://en.wikipedia.org/wiki/Standard_conditions_for_temperature_and_pressure
	uint64_t lastTimestamp = 0;
	float lastAltitude = 0;
	Averager averageClimb;
	Averager averageAltitude;
	Signal<float> climbSignl;
	Signal<float> altitudeSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
