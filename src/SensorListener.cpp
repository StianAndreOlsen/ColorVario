#include "SensorListener.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::SensorListener::~SensorListener() noexcept
{
	try { stop(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { unsetSensorEventCallback(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::SensorListener::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
}

void Kystsoft::SensorListener::start()
{
	if (started)
		return;
	int error = sensor_listener_start(listener);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_listener_start", error);
	started = true;
}

void Kystsoft::SensorListener::stop()
{
	if (!started)
		return;
	int error = sensor_listener_stop(listener);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_listener_stop", error);
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
		throw TizenError("sensor_listener_set_interval", error);
}

void Kystsoft::SensorListener::setAttribute(sensor_attribute_e attribute, int value)
{
	int error = sensor_listener_set_attribute_int(listener, attribute, value);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_listener_set_attribute_int", error);
}

void Kystsoft::SensorListener::createAndSetSensorEventCallback(Sensor sensor)
{
	create(sensor);
	try
	{
		setSensorEventCallback();
	}
	catch (...)
	{
		destroy();
		throw;
	}
}

void Kystsoft::SensorListener::create(Sensor sensor)
{
	int error = sensor_create_listener(sensor, &listener);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_create_listener", error);
}

void Kystsoft::SensorListener::destroy()
{
	int error = sensor_destroy_listener(listener);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_destroy_listener", error);
	listener = nullptr;
}

void Kystsoft::SensorListener::setSensorEventCallback()
{
	int error = sensor_listener_set_event_cb(listener, 0, sensorEventCallback, this);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_listener_set_event_cb", error);
}

void Kystsoft::SensorListener::unsetSensorEventCallback()
{
	int error = sensor_listener_unset_event_cb(listener);
	if (error != SENSOR_ERROR_NONE)
		throw TizenError("sensor_listener_unset_event_cb", error);
}

void Kystsoft::SensorListener::sensorEventCallback(sensor_h sensor, sensor_event_s* event, void* user_data)
{
	SensorListener* listener = static_cast<SensorListener*>(user_data);
	if (listener)
		listener->onSensorEvent(sensor, event);
}
