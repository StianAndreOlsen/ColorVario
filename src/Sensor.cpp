#include "Sensor.h"
#include "TizenError.h"

bool Kystsoft::Sensor::isSupported(sensor_type_e type)
{
	bool supported = false;
	int error = sensor_is_supported(type, &supported);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_is_supported", error);
	return supported;
}

bool Kystsoft::Sensor::isWakeUp() const
{
	bool wakeUp = false;
	int error = sensor_is_wake_up(sensor, &wakeUp);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_is_wake_up", error);
	return wakeUp;
}

sensor_h Kystsoft::Sensor::defaultSensor(sensor_type_e type)
{
	sensor_h sensor = nullptr;
	int error = sensor_get_default_sensor(type, &sensor);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_default_sensor", error);
	return sensor;
}

std::vector<Kystsoft::Sensor> Kystsoft::Sensor::list(sensor_type_e type /*= SENSOR_ALL*/)
{
	sensor_h* list = nullptr;
	int count = 0;
	int error = sensor_get_sensor_list(type, &list, &count);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_sensor_list", error);
	std::vector<Sensor> sensorList(list, list + count);
	free(list);
	return sensorList;
}

std::string Kystsoft::Sensor::name() const
{
	char* name = nullptr;
	int error = sensor_get_name(sensor, &name);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_name", error);
	std::string sensorName(name);
	free(name);
	return sensorName;
}

std::string Kystsoft::Sensor::vendor() const
{
	char* vendor = nullptr;
	int error = sensor_get_vendor(sensor, &vendor);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_vendor", error);
	std::string sensorVendor(vendor);
	free(vendor);
	return sensorVendor;
}

sensor_type_e Kystsoft::Sensor::type() const
{
	sensor_type_e type = SENSOR_ALL;
	int error = sensor_get_type(sensor, &type);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_type", error);
	return type;
}

float Kystsoft::Sensor::minRange() const
{
	float range = 0;
	int error = sensor_get_min_range(sensor, &range);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_min_range", error);
	return range;
}

float Kystsoft::Sensor::maxRange() const
{
	float range = 0;
	int error = sensor_get_max_range(sensor, &range);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_max_range", error);
	return range;
}

float Kystsoft::Sensor::resolution() const
{
	float resolution = 0;
	int error = sensor_get_resolution(sensor, &resolution);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_resolution", error);
	return resolution;
}

int Kystsoft::Sensor::minInterval() const
{
	int interval = 0;
	int error = sensor_get_min_interval(sensor, &interval);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_get_min_interval", error);
	return interval;
}
