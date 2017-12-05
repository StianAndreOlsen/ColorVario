#ifndef KYSTSOFT_SENSORLISTENER_H
#define KYSTSOFT_SENSORLISTENER_H

#include "Sensor.h"
#include "Signal.h"

namespace Kystsoft {

class SensorListener
{
public:
	SensorListener(Sensor sensor) { create(sensor); }
	SensorListener(sensor_h handle) { create(handle); }
	SensorListener(sensor_type_e type) { create(type); }
	SensorListener(const SensorListener& other) = delete;
	SensorListener& operator=(const SensorListener& rhs) = delete;
	~SensorListener() noexcept { sensor_destroy_listener(listener); }
	operator sensor_listener_h() const { return listener; }
	bool isStarted() const { return started; }
	void start();
	void stop();
	void toggleStartStop();
	void setInterval(uint32_t interval_ms);
	void setAttribute(sensor_attribute_e attribute, int value);
	using EventSignal = Signal<Sensor, sensor_event_s*>;
	const EventSignal& eventSignal() const { return eventSignl; }
private:
	void create(Sensor sensor);
	static void sensorEventCallback(sensor_h sensor, sensor_event_s* event, void* user_data);
	void onSensorEvent(sensor_h sensor, sensor_event_s* event) { eventSignl.emit(sensor, event); }
	sensor_listener_h listener = nullptr;
	bool started = false;
	EventSignal eventSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SENSORLISTENER_H
