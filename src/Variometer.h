#ifndef KYSTSOFT_VARIOMETER_H
#define KYSTSOFT_VARIOMETER_H

#include "SensorListener.h"
#include "Settings.h"
#include <limits>

namespace Kystsoft {

class Variometer
{
public:
	Variometer();
	Variometer(const Variometer& other) = delete;
	Variometer& operator=(const Variometer& rhs) = delete;
	void load(const Settings& settings);
	static double pressureAltitude(double pressure, double referencePressure);
	static double referencePressure(double pressureAltitude, double pressure);
	double samplingInterval() const { return interval_ms / 1000.0; }
	void setSamplingInterval(double interval);
	bool isStarted() const { return pressureListener.isStarted(); }
	void setStarted(bool started);
	void start();
	void stop() { pressureListener.stop(); }
	void toggleStartStop();
	void setCurrentAltitude(double altitude) { currentAltitude = altitude; } // calibrates the altimeter
	double altitudeOffset() const { return altitudeOffst; }
	void setAltitudeOffset(double offset) { altitudeOffst = offset; }
	const auto& climbSignal() const { return climbSignl; }
	const auto& altitudeSignal() const { return altitudeSignl; }
private:
	void onPressureSensorEvent(Sensor sensor, sensor_event_s* event);
	SensorListener pressureListener;
	uint32_t interval_ms = 100;
	double currentAltitude = -std::numeric_limits<double>::max();
	double refPressure = 1013.25; // https://en.wikipedia.org/wiki/Standard_conditions_for_temperature_and_pressure
	uint64_t lastTimestamp = 0;
	double lastAltitude = 0;
	double altitudeOffst = 0;
	Signal<double> climbSignl;
	Signal<double> altitudeSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOMETER_H
