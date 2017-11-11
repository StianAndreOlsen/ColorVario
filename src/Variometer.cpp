#include "Variometer.h"
#include <dali-toolkit/dali-toolkit.h>
#include <tone_player.h>
#include <dlog.h>
#include <stdio.h>

namespace
{
	Dali::Toolkit::TextLabel* climbLabel = NULL;
	Dali::Toolkit::TextLabel* altitudeLabel = NULL;
}

void Kystsoft::Variometer::SetClimbLabel(Dali::Toolkit::TextLabel* label)
{
	climbLabel = label;
}

void Kystsoft::Variometer::SetAltitudeLabel(Dali::Toolkit::TextLabel* label)
{
	altitudeLabel = label;
}

void PressureCallback(sensor_h sensor, sensor_event_s* event, void* data)
{
	static float altitudePrev = 0;
	float altitude = event->values[1];
	if (altitudeLabel)
	{
		char temp[50] = ""; // TODO: Replace with std::ostringstream
		snprintf(temp, sizeof(temp), "%.2f m", altitude);
		altitudeLabel->SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, temp);
	}
	float climb = 0;
	if (altitudePrev != 0)
		climb = (altitude - altitudePrev) * 10;
	if (climbLabel)
	{
		char temp[50] = "";
		snprintf(temp, sizeof(temp), "%.2f m/s", climb);
		climbLabel->SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, temp);
	}
	altitudePrev = altitude;
//	int tone_player = 0;
//	tone_player_start_new(TONE_TYPE_USER_DEFINED_LOW_FRE, SOUND_TYPE_MEDIA, 1000, &tone_player);
}

int Kystsoft::Variometer::Start()
{
	if (IsStarted())
		return SENSOR_ERROR_NONE;

	bool supported = false;
	if (sensor_is_supported(SENSOR_PRESSURE, &supported) != SENSOR_ERROR_NONE || !supported)
		return 1;

	if (sensor_get_default_sensor(SENSOR_PRESSURE, &pressureSensor) != SENSOR_ERROR_NONE)
		return 2;

	if (sensor_create_listener(pressureSensor, &pressureListener) != SENSOR_ERROR_NONE)
		return 3;

	if (sensor_listener_set_event_cb(pressureListener, 100, &PressureCallback, NULL) != SENSOR_ERROR_NONE)
		return 4;

	if (sensor_listener_start(pressureListener) != SENSOR_ERROR_NONE)
		return 5;

	return SENSOR_ERROR_NONE;
}

void Kystsoft::Variometer::Stop()
{
	if (!IsStarted())
		return;

	sensor_listener_stop(pressureListener);
	sensor_destroy_listener(pressureListener);
	pressureListener = NULL;
}
