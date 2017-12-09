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

	// calculate altitude
	// TODO: Consider getting actual temperature from sensor
//	float t0 = 15; // reference temperature [°C]
	// TODO: Consider letting the user calibrate the height by changing the reference pressure
	float p0 = 1013.25f; // reference pressure [hPa]
//	float altitude = (std::pow(p0 / pressure, 1 / 5.257f) - 1) * (273.15f + t0) / 0.0065f; // https://physics.stackexchange.com/questions/333475/how-to-calculate-altitude-from-current-temperature-and-pressure
	float altitude = (1 - std::pow(pressure / p0, 0.190284f)) * 145366.45f; // https://en.wikipedia.org/wiki/Pressure_altitude
	altitude *= 0.3048f; // convert from feet to meter
	// TODO: Consider using the below library function when available in version 4.0
//	sensor_util_get_altitude(pressure, p0, t0, &altitude);
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
