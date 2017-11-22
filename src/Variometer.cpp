#include "Variometer.h"
#include <sstream>
#include <vector>

namespace {

void pressureCallback(sensor_h /*sensor*/, sensor_event_s* event, void* user_data)
{
	Kystsoft::Variometer* vario = static_cast<Kystsoft::Variometer*>(user_data);
	if (vario)
		vario->onPressureChange(event->timestamp, event->values[0]);
}

void audioCallback(audio_out_h /*handle*/, size_t nbytes, void* user_data)
{
	Kystsoft::Variometer* vario = static_cast<Kystsoft::Variometer*>(user_data);
	if (vario)
		vario->onAudioRequested(nbytes);
}

} // namespace

int Kystsoft::Variometer::start()
{
	if (isStarted())
		return 0;

	// start pressure sensor listener
	bool supported = false;
	if (sensor_is_supported(SENSOR_PRESSURE, &supported) != SENSOR_ERROR_NONE || !supported)
		return 1;
	sensor_h pressureSensor = nullptr;
	if (sensor_get_default_sensor(SENSOR_PRESSURE, &pressureSensor) != SENSOR_ERROR_NONE)
		return 2;
	if (sensor_create_listener(pressureSensor, &pressureListener) != SENSOR_ERROR_NONE)
		return 3;
	if (sensor_listener_set_event_cb(pressureListener, 100, pressureCallback, this) != SENSOR_ERROR_NONE)
		return 4;
	if (sensor_listener_start(pressureListener) != SENSOR_ERROR_NONE)
		return 5;

	// start sound
	const int sampleRate = 44100; // standard CD sample rate
	if (audio_out_create_new(sampleRate, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8, &audioOutput) != AUDIO_IO_ERROR_NONE)
		return 6;
	if (audio_out_set_stream_cb(audioOutput, audioCallback, this) != AUDIO_IO_ERROR_NONE)
		return 7;
	if (sound.isSoundOn() && !turnSoundOn())
		return 8;

	return 0;
}

void Kystsoft::Variometer::stop()
{
	if (!isStarted())
		return;

	// stop pressure sensor listener
	sensor_listener_stop(pressureListener);
	sensor_destroy_listener(pressureListener);
	pressureListener = nullptr;

	// stop sound
	if (sound.isSoundOn())
		turnSoundOff();
	audio_out_unset_stream_cb(audioOutput);
	audio_out_destroy(audioOutput);
	audioOutput = nullptr;
}

void Kystsoft::Variometer::onPressureChange(unsigned long long timestamp, float pressure)
{
	// calculate altitude
	// TODO: Consider getting actual temperature from sensor
//	float t0 = 15; // reference temperature [°C]
	// TODO: Consider letting the user calibrate the height by changing the reference pressure
	float p0 = 1013.25f; // reference pressure [hPa]
//	float altitude = (std::pow(p0 / pressure, 1 / 5.257f) - 1) * (273.15f + t0) / 0.0065f; // https://physics.stackexchange.com/questions/333475/how-to-calculate-altitude-from-current-temperature-and-pressure
	float altitude = (1 - std::pow(pressure / p0, 0.190284f)) * 145366.45f; // https://en.wikipedia.org/wiki/Pressure_altitude
	altitude *= 0.3048f; // convert from ft to m
	// TODO: Consider using the below library function when available in version 4.0
//	sensor_util_get_altitude(pressure, p0, t0, &altitude);
	averageAltitude += altitude;

	// calculate climb
	float seconds = (timestamp - lastTimestamp) / 1000000.0f;
	float climb = (altitude - lastAltitude) / seconds;
	climb = 12 + (pressure - 260) * (-12 - 12) / (1260 - 260); // TODO: Remove after debugging
	averageClimb += climb;

	// turn sound on or off
	bool soundWasOn = sound.isSoundOn();
	sound.setClimb(averageClimb);
	bool soundIsOn = sound.isSoundOn();
	if (!soundWasOn && soundIsOn)
		turnSoundOn();
	else if (soundWasOn && !soundIsOn)
		turnSoundOff();

	// update labels
	if (climbLabel)
	{
		std::ostringstream os;
		os.setf(std::ios::fixed);
		os.precision(2); // TODO: Reduce to 1?
		os << averageClimb << " m/s";
		climbLabel->SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
	}
	if (altitudeLabel)
	{
		std::ostringstream os;
		os.setf(std::ios::fixed);
		os.precision(2); // TODO: Reduce to 0?
		os << averageAltitude << " m";
		altitudeLabel->SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, os.str());
	}

	// save values
	lastTimestamp = timestamp;
	lastAltitude = altitude;
}

void Kystsoft::Variometer::onAudioRequested(size_t bytesRequested)
{
	// get sample rate
	int sampleRate = 0;
	audio_out_get_sample_rate(audioOutput, &sampleRate);

	// get sound characteristics
	float frequency = sound.frequency();
	float period = sound.period();
	float duty = sound.duty();

	// calculate number of points in a full cycle
	uint32_t cyclePointCount = uint32_t(sampleRate * period);

	// create sound and silence points
	size_t pointCount = bytesRequested; // 8-bit audio --> number of points equals number of bytes
	std::vector<uint8_t> points(pointCount, INT8_MAX);
	float cyclePhase = 0;
	float tonePhase = 0;
	for (size_t i = 0; i < pointCount; ++i)
	{
		cyclePhase = lastCyclePhase + float(i) / cyclePointCount;
		if (cyclePhase >= 1)
			cyclePhase -= int(cyclePhase); // keep below 1
		if (cyclePhase < duty)
		{
			float x = lastTonePhase + i * frequency / sampleRate;
//			float y = std::sin(2 * 3.14159265359f * x); // sine wave (https://en.wikipedia.org/wiki/Sine_wave)
			float y = 2 * (x - std::floor(x + 0.5f)); // sawtooth wave (https://en.wikipedia.org/wiki/Sawtooth_wave)
//			float y = 2 * std::abs(2 * (x + 0.25f - std::floor(x + 0.75f))) - 1; // triangle wave (https://en.wikipedia.org/wiki/Triangle_wave)
//			float y = 2 * (2 * std::floor(x) - std::floor(2 * x)) + 1; // square wave (https://en.wikipedia.org/wiki/Square_wave)
			points[i] = uint8_t(y * INT8_MAX + INT8_MAX);
			tonePhase = x - int(x); // keep below 1
		}
		else
		{
			lastTonePhase = tonePhase = 0;
		}
	}
	lastCyclePhase = cyclePhase;
	lastTonePhase = tonePhase;

	// play sound
	audio_out_write(audioOutput, points.data(), bytesRequested);
}

bool Kystsoft::Variometer::turnSoundOn()
{
	if (audio_out_prepare(audioOutput) == AUDIO_IO_ERROR_NONE)
	{
		lastCyclePhase = 0;
		lastTonePhase = 0;
		return true;
	}
	return false;
}

bool Kystsoft::Variometer::turnSoundOff()
{
	audio_out_flush(audioOutput); // TODO: Required?
	return audio_out_unprepare(audioOutput) == AUDIO_IO_ERROR_NONE;
}
