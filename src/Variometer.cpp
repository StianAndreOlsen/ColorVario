#include "Variometer.h"
#include <cmath>

Kystsoft::Variometer::Variometer()
	: pressureListener(SENSOR_PRESSURE)
	, averageClimb(5)
	, averageAltitude(5)
{
	pressureListener.setInterval(interval_ms);
	pressureListener.setAttribute(SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	pressureListener.eventSignal().connect(this, &Variometer::onPressureSensorEvent);
}

void Kystsoft::Variometer::load(const Settings& settings)
{
	setSamplingInterval(settings.value("Variometer.samplingInterval", samplingInterval()));
	setAveragingInterval(settings.value("Variometer.averagingInterval", averagingInterval()));
}

void Kystsoft::Variometer::setSamplingInterval(float interval)
{
	uint32_t newInterval = uint32_t(interval * 1000 + 0.5f);
	if (newInterval < 10)
		newInterval = 10;
	if (interval_ms != newInterval)
	{
		interval_ms = newInterval;
		pressureListener.setInterval(interval_ms);
	}
}

void Kystsoft::Variometer::setAveragingInterval(float interval)
{
	size_t size = size_t(interval / samplingInterval() + 0.5f);
	if (size < 1)
		size = 1;
	if (averageClimb.size() != size)
	{
		averageClimb.resize(size);
		averageAltitude.resize(size);
	}
}

void Kystsoft::Variometer::onPressureSensorEvent(Sensor /*sensor*/, sensor_event_s* event)
{
	// get event values
	uint64_t timestamp = event->timestamp;
	float pressure = event->values[0];

	// calculate altitude (https://en.wikipedia.org/wiki/Pressure_altitude)
	float altitude = (1 - std::pow(pressure / referencePressure, 0.190284f)) * 145366.45f;
	altitude *= 0.3048f; // convert from feet to meter

	// calculate climb
	float seconds = (timestamp - lastTimestamp) / 1000000.0f;
	float climb = (altitude - lastAltitude) / seconds;
//	climb = 12 + (pressure - 260) * (-12 - 12) / (1260 - 260); // TODO: Remove after debugging

	// calibrate altimeter
	if (currentAltitude > -6.4e+6f)
	{
		// calculate reference pressure
		currentAltitude /= 0.3048f; // convert from meter to feet
		referencePressure = pressure / std::pow(1 - currentAltitude / 145366.45f, 1 / 0.190284f);

		// recalculate altitude (https://en.wikipedia.org/wiki/Pressure_altitude)
		altitude = (1 - std::pow(pressure / referencePressure, 0.190284f)) * 145366.45f;
		altitude *= 0.3048f; // convert from feet to meter

		// flag calibration as finished
		currentAltitude = -6.5e+6f;
	}

	// add to average values
	averageClimb += climb;
	averageAltitude += altitude;

	// save to next event
	lastTimestamp = timestamp;
	lastAltitude = altitude;

	// emit values
	climbSignl.emit(averageClimb);
	altitudeSignl.emit(averageAltitude);
}
