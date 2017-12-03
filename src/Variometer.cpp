#include "Variometer.h"
#include <cmath>

Kystsoft::Variometer::Variometer()
	: pressureListener(SENSOR_PRESSURE)
	, averageClimb(5)
	, averageAltitude(5)
	, audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8)
{
//	pressureListener.setInterval(100);
	pressureListener.setAttribute(SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	pressureListener.eventSignal().connect(this, &Variometer::onPressureSensorEvent);
	audioOutput.setStreamWriter([this](AudioOutput& audioOutput, size_t bytesRequested)
	{
		onAudioRequested(audioOutput, bytesRequested);
	});
}

void Kystsoft::Variometer::start()
{
	if (isStarted())
		return;
	pressureListener.start();

	// start sound
	if (sound.isSoundOn())
		turnSoundOn();
}

void Kystsoft::Variometer::stop()
{
	if (!isStarted())
		return;
	pressureListener.stop();

	// stop sound
	if (sound.isSoundOn())
		turnSoundOff();
}

void Kystsoft::Variometer::onPressureSensorEvent(sensor_event_s* event)
{
	// get timestamp and pressure
	uint64_t timestamp = event->timestamp;
	float pressure = event->values[0];

	// save old values
	float oldClimb = averageClimb;
	float oldAltitude = averageAltitude;

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
	float newAltitude = averageAltitude;

	// calculate climb
	float seconds = (timestamp - lastTimestamp) / 1000000.0f;
	float climb = (altitude - lastAltitude) / seconds;
	climb = 12 + (pressure - 260) * (-12 - 12) / (1260 - 260); // TODO: Remove after debugging
	averageClimb += climb;
	float newClimb = averageClimb;

	// save values
	lastTimestamp = timestamp;
	lastAltitude = altitude;

	// TODO: Consider skipping the inequality test
	if (newClimb != oldClimb)
	{
		// turn sound on or off
		bool soundWasOn = sound.isSoundOn();
		sound.setClimb(newClimb);
		bool soundIsOn = sound.isSoundOn();
		if (!soundWasOn && soundIsOn)
			turnSoundOn();
		else if (soundWasOn && !soundIsOn)
			turnSoundOff();

		climbSignl.emit(newClimb);
	}

	// TODO: Consider skipping the inequality test
	if (newAltitude != oldAltitude)
	{
		altitudeSignl.emit(newAltitude);
	}
}

void Kystsoft::Variometer::onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested)
{
	// get sample rate
	int sampleRate = audioOutput.sampleRate();

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
	audioOutput.write(points);
}

void Kystsoft::Variometer::turnSoundOn()
{
	lastCyclePhase = 0;
	lastTonePhase = 0;
	audioOutput.prepare();
}

void Kystsoft::Variometer::turnSoundOff()
{
	audioOutput.unprepare();
}
