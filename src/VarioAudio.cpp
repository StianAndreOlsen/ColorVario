#include "VarioAudio.h"
#include <cmath>

Kystsoft::VarioAudio::VarioAudio()
// TODO: Figure out the native audio format used by the watch and choose that, as recommended here:
// https://developer.tizen.org/ko/development/guides/native-application/media-and-camera/raw-audio-playback-and-recording
//	: audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8)
//	: audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_S16_LE)
//	: audioOutput(44100, AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_TYPE_U8)
	: audioOutput(44100, AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_TYPE_S16_LE)
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
	// get audio characteristics
	int sampleRate = audioOutput.sampleRate();
	audio_channel_e channel = audioOutput.channel();
	audio_sample_type_e sampleType = audioOutput.sampleType();

	// create silence points
	if (isSoundOff())
	{
		lastCyclePhase = 0;
		lastTonePhase = 0;
		if (sampleType == AUDIO_SAMPLE_TYPE_U8)
		{
			std::vector<uint8_t> points(bytesRequested, INT8_MAX);
			audioOutput.write(points);
		}
		else // AUDIO_SAMPLE_TYPE_S16_LE
		{
			std::vector<int16_t> points(bytesRequested / 2, 0);
			audioOutput.write(points);
		}
		return;
	}

	// get sound characteristics
	float frequency = sound.frequency(currentClimb);
	float period = sound.period(currentClimb);
	float duty = sound.duty(currentClimb);

	// calculate number of points
	size_t pointCount = bytesRequested;
	if (channel == AUDIO_CHANNEL_STEREO)
		pointCount /= 2;
	if (sampleType == AUDIO_SAMPLE_TYPE_S16_LE)
		pointCount /= 2;

	// calculate number of points in a full cycle
	uint32_t cyclePointCount = uint32_t(sampleRate * period);

	// create sound and silence points
	std::vector<uint8_t> pointsU8;
	std::vector<int16_t> points16;
	size_t size = pointCount;
	if (channel == AUDIO_CHANNEL_STEREO)
		size *= 2;
	if (sampleType == AUDIO_SAMPLE_TYPE_U8)
		pointsU8.resize(size, INT8_MAX);
	else // AUDIO_SAMPLE_TYPE_S16_LE
		points16.resize(size, 0);
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
			if (sampleType == AUDIO_SAMPLE_TYPE_U8)
			{
				uint8_t point = uint8_t(y * INT8_MAX + INT8_MAX);
				if (channel == AUDIO_CHANNEL_STEREO)
					pointsU8[2*i] = pointsU8[2*i+1] = point;
				else
					pointsU8[i] = point;
			}
			else // AUDIO_SAMPLE_TYPE_S16_LE
			{
				int16_t point = int16_t(y * INT16_MAX);
				if (channel == AUDIO_CHANNEL_STEREO)
					points16[2*i] = points16[2*i+1] = point;
				else
					points16[i] = point;
			}
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
	if (sampleType == AUDIO_SAMPLE_TYPE_U8)
		audioOutput.write(pointsU8);
	else
		audioOutput.write(points16);
}
