#include "SensorListener.h"
#include "FunctionError.h"

void Kystsoft::SensorListener::start()
{
	if (started)
		return;
	int error = sensor_listener_start(listener);
	if (error != SENSOR_ERROR_NONE)
		throw FunctionError("sensor_listener_start", error);
	started = true;
}

void Kystsoft::SensorListener::stop()
{
	if (!started)
		return;
	int error = sensor_listener_stop(listener);
	if (error != SENSOR_ERROR_NONE)
		throw FunctionError("sensor_listener_stop", error);
	started = false;
}

void Kystsoft::SensorListener::toggleStartStop()
{
	if (started)
		stop();
	else
		start();
}

void Kystsoft::SensorListener::setInterval(uint32_t interval_ms)
{
	int error = sensor_listener_set_interval(listener, interval_ms);
	if (error != SENSOR_ERROR_NONE)
		throw FunctionError("sensor_listener_set_interval", error);
}

void Kystsoft::SensorListener::setAttribute(sensor_attribute_e attribute, int value)
{
	int error = sensor_listener_set_attribute_int(listener, attribute, value);
	if (error != SENSOR_ERROR_NONE)
		throw FunctionError("sensor_listener_set_attribute_int", error);
}

void Kystsoft::SensorListener::create(Sensor sensor)
{
	int error = sensor_create_listener(sensor, &listener);
	if (error != SENSOR_ERROR_NONE)
		throw FunctionError("sensor_create_listener", error);

	error = sensor_listener_set_event_cb(listener, 0, sensorEventCallback, this);
	if (error != SENSOR_ERROR_NONE)
	{
		sensor_destroy_listener(listener);
		listener = nullptr;
		throw FunctionError("sensor_listener_set_event_cb", error);
	}
}

void Kystsoft::SensorListener::sensorEventCallback(sensor_h sensor, sensor_event_s* event, void* user_data)
{
	SensorListener* listener = static_cast<SensorListener*>(user_data);
	if (listener != nullptr)
		listener->onSensorEvent(sensor, event);
}
