#include "Variometer.h"
#include <cmath>

Kystsoft::Variometer::Variometer()
	: pressureListener(SENSOR_PRESSURE)
	, averageClimb(5)
	, averageAltitude(5)
{
	pressureListener.setInterval(100);
	pressureListener.setAttribute(SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	pressureListener.eventSignal().connect(this, &Variometer::onPressureSensorEvent);
}

void Kystsoft::Variometer::onPressureSensorEvent(Sensor /*sensor*/, sensor_event_s* event)
{
	// get event values
	uint64_t timestamp = event->timestamp;
	float pressure = event->values[0];

	// calculate reference pressure (calibrate altimeter)
	if (currentAltitude > -6.4e+6f)
	{
		currentAltitude /= 0.3048f; // convert from meter to feet
		referencePressure = pressure / std::pow(1 - currentAltitude / 145366.45f, 1 / 0.190284f);
		currentAltitude = -6.5e+6f;
	}

	// calculate altitude (https://en.wikipedia.org/wiki/Pressure_altitude)
	float altitude = (1 - std::pow(pressure / referencePressure, 0.190284f)) * 145366.45f;
	altitude *= 0.3048f; // convert from feet to meter
	averageAltitude += altitude;

	// calculate climb
	float seconds = (timestamp - lastTimestamp) / 1000000.0f;
	float climb = (altitude - lastAltitude) / seconds;
	climb = 12 + (pressure - 260) * (-12 - 12) / (1260 - 260); // TODO: Remove after debugging
	averageClimb += climb;

	// save to next event
	lastTimestamp = timestamp;
	lastAltitude = altitude;

	// emit values
	climbSignl.emit(averageClimb);
	altitudeSignl.emit(averageAltitude);
}
