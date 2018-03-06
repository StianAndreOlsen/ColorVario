#include "VarioAudio.h"
#include <cmath>

Kystsoft::VarioAudio::VarioAudio()
	: audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8)
{
	audioOutput.writeCallback().connect(this, &VarioAudio::onAudioRequested);
}

void Kystsoft::VarioAudio::load(const Settings& settings)
{
	setMuted(settings.value("Sound.muted", false));
	sound.load(settings);
}

void Kystsoft::VarioAudio::setMuted(bool muted)
{
	if (muted)
		mute();
	else
		unmute();
}

void Kystsoft::VarioAudio::mute()
{
	if (isMuted())
		return;
	audioOutput.unprepare();
	mutedSignl.emit(true);
}

void Kystsoft::VarioAudio::unmute()
{
	if (!isMuted())
		return;
	audioOutput.prepare();
	mutedSignl.emit(false);
}

void Kystsoft::VarioAudio::toggleMuteUnmute()
{
	if (isMuted())
		unmute();
	else
		mute();
}

void Kystsoft::VarioAudio::setClimb(float climb)
{
	if (currentClimb == climb)
		return;
	float previousClimb = currentClimb;
	currentClimb = climb;

	// sound on or off
	if (previousClimb > 3e+8f)
	{
		// special handling if this is the first real climb value
		if (currentClimb >= sound.climbSoundOnThreshold())
			soundOn = 1; // turn on climb sound
		else if (currentClimb <= sound.sinkSoundOnThreshold())
			soundOn = -1; // turn on sink sound
	}
	else if (currentClimb > previousClimb)
	{
		if (isSinkSoundOn() && currentClimb >= sound.sinkSoundOffThreshold())
			soundOn = 0; // turn off sink sound
		if (isSoundOff() && currentClimb >= sound.climbSoundOnThreshold())
			soundOn = 1; // turn on climb sound
	}
	else // remember that equality is tested at function start
	{
		if (isClimbSoundOn() && currentClimb <= sound.climbSoundOffThreshold())
			soundOn = 0; // turn off climb sound
		if (isSoundOff() && currentClimb <= sound.sinkSoundOnThreshold())
			soundOn = -1; // turn on sink sound
	}
}

void Kystsoft::VarioAudio::onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested)
{
	if (!isSoundOn())
	{
		lastCyclePhase = 0;
		lastTonePhase = 0;
		size_t pointCount = bytesRequested; // 8-bit audio --> number of points equals number of bytes
		std::vector<uint8_t> points(pointCount, INT8_MAX);
		audioOutput.write(points);
		return;
	}

	// get sample rate
	int sampleRate = audioOutput.sampleRate();

	// get sound characteristics
	float frequency = sound.frequency(currentClimb);
	float period = sound.period(currentClimb);
	float duty = sound.duty(currentClimb);

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
			float y = 0;
			switch (sound.waveform())
			{
			case Waveform::Sine: // sine wave (https://en.wikipedia.org/wiki/Sine_wave)
				y = std::sin(2 * 3.14159265359f * x);
				break;
			case Waveform::Square: // square wave (https://en.wikipedia.org/wiki/Square_wave)
				y = 2 * (2 * std::floor(x) - std::floor(2 * x)) + 1;
				break;
			case Waveform::Triangle: // triangle wave (https://en.wikipedia.org/wiki/Triangle_wave)
				y = 2 * std::abs(2 * (x + 0.25f - std::floor(x + 0.75f))) - 1;
				break;
			case Waveform::Sawtooth: // sawtooth wave (https://en.wikipedia.org/wiki/Sawtooth_wave)
				y = 2 * (x - std::floor(x + 0.5f));
				break;
			}
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
