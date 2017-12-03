#ifndef KYSTSOFT_SENSOR_H
#define KYSTSOFT_SENSOR_H

#include <sensor.h>
#include <string>
#include <vector>

namespace Kystsoft {

class Sensor
{
public:
	Sensor(sensor_h handle = nullptr) : sensor(handle) {}
	Sensor(sensor_type_e type) : sensor(defaultSensor(type)) {}
	operator sensor_h() const { return sensor; }
	bool isNull() const { return sensor == nullptr; }
	static bool isSupported(sensor_type_e type);
	bool isWakeUp() const;
	static sensor_h defaultSensor(sensor_type_e type);
	static std::vector<Sensor> list(sensor_type_e type = SENSOR_ALL);
	std::string name() const;
	std::string vendor() const;
	sensor_type_e type() const;
	float minRange() const;
	float maxRange() const;
	float resolution() const;
	int minInterval() const;
private:
	sensor_h sensor = nullptr;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SENSOR_H
