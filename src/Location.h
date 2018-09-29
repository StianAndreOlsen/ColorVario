#ifndef KYSTSOFT_LOCATION_H
#define KYSTSOFT_LOCATION_H

#include <locations.h>
#include <ctime>

namespace Kystsoft {

struct Location
{
	double latitude = 0; // degrees [-90, 90]
	double longitude = 0; // degrees [-180, 180]
	double altitude = 0; // altitude above the geodetic datum [m]
	double speed = 0; // horizontal speed [m/s]
	double direction = 0; // traveling direction in degrees [0, 360) (north 0, east 90, south 180, west 270)
	double climb = 0; // vertical speed [m/s]
	location_accuracy_level_e level = LOCATIONS_ACCURACY_NONE;
	double horizontal = 0; // horizontal accuracy [m]
	double vertical = 0; // vertical accuracy [m]
	time_t timestamp = 0; // time when the measurement took place (or 0 if invalid)
};

} // namespace Kystsoft

#endif // KYSTSOFT_LOCATION_H
