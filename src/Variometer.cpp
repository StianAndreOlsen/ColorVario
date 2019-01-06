#include "Variometer.h"
#include <cmath>

Kystsoft::Variometer::Variometer()
	: pressureListener(SENSOR_PRESSURE)
{
	pressureListener.setInterval(interval_ms);
	pressureListener.setAttribute(SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	pressureListener.eventSignal().connect(this, &Variometer::onPressureSensorEvent);
}

void Kystsoft::Variometer::load(const Settings& settings)
{
	setSamplingInterval(settings.value("Variometer.samplingInterval", samplingInterval()));
	setAltitudeOffset(settings.value("Variometer.altitudeOffset", altitudeOffset()));
}

// https://en.wikipedia.org/wiki/Pressure_altitude
double Kystsoft::Variometer::pressureAltitude(double pressure, double referencePressure)
{
	double pressureAltitude = (1 - std::pow(pressure / referencePressure, 0.190284)) * 145366.45;
	pressureAltitude *= 0.3048; // convert from feet to meters
	return pressureAltitude;
}

// https://en.wikipedia.org/wiki/Pressure_altitude
double Kystsoft::Variometer::referencePressure(double pressureAltitude, double pressure)
{
	pressureAltitude /= 0.3048; // convert from meters to feet
	double referencePressure = pressure / std::pow(1 - pressureAltitude / 145366.45, 1 / 0.190284);
	return referencePressure;
}

void Kystsoft::Variometer::setSamplingInterval(double interval)
{
	uint32_t newInterval = uint32_t(interval * 1000 + 0.5);
	if (newInterval < 10)
		newInterval = 10;
	if (interval_ms != newInterval)
	{
		interval_ms = newInterval;
		pressureListener.setInterval(interval_ms);
	}
}

void Kystsoft::Variometer::setStarted(bool started)
{
	if (!isStarted() && started)
		lastTimestamp = 0;
	pressureListener.setStarted(started);
}

void Kystsoft::Variometer::start()
{
	if (!isStarted())
		lastTimestamp = 0;
	pressureListener.start();
}

void Kystsoft::Variometer::toggleStartStop()
{
	if (!isStarted())
		lastTimestamp = 0;
	pressureListener.toggleStartStop();
}

void Kystsoft::Variometer::onPressureSensorEvent(Sensor /*sensor*/, sensor_event_s* event)
{
	// get event values
	uint64_t timestamp = event->timestamp;
	double pressure = event->values[0];

	// calculate altitude
	double altitude = pressureAltitude(pressure, refPressure);

	// check if this is the first sensor event
	if (lastTimestamp == 0)
	{
		lastTimestamp = timestamp;
		lastAltitude = altitude;
		return;
	}

	// calculate climb
	double seconds = (timestamp - lastTimestamp) / 1000000.0;
	double climb = (altitude - lastAltitude) / seconds;
//	climb = 12 + (pressure - 260) * (-12 - 12) / (1260 - 260); // TODO: Remove after debugging

	// calibrate altimeter
	if (currentAltitude > -std::numeric_limits<double>::max())
	{
		// calculate reference pressure
		refPressure = referencePressure(currentAltitude, pressure);

		// recalculate altitude
		altitude = pressureAltitude(pressure, refPressure);

		// flag calibration as finished
		currentAltitude = -std::numeric_limits<double>::max();
	}

	// save to next event
	lastTimestamp = timestamp;
	lastAltitude = altitude;

	// emit values
	climbSignl.emit(climb);
	altitudeSignl.emit(altitude + altitudeOffst);
}
