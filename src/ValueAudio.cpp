#include "ValueAudio.h"
#include "dlog.h"
#include <cmath>

Kystsoft::ValueAudio::ValueAudio()
	: soundStream(SOUND_STREAM_TYPE_MEDIA)
// TODO: Figure out the native audio format used by the watch and choose that, as recommended here:
// https://developer.tizen.org/ko/development/guides/native-application/media-and-camera/raw-audio-playback-and-recording
//	, audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8)
//	, audioOutput(44100, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_S16_LE)
//	, audioOutput(44100, AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_TYPE_U8)
	, audioOutput(44100, AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_TYPE_S16_LE)
{
	audioOutput.setSoundStreamInfo(soundStream);
	audioOutput.writeCallback().connect(this, &ValueAudio::onAudioRequested);
	mutedId = mutedSignal.connect(this, &ValueAudio::stop);
}

Kystsoft::ValueAudio::~ValueAudio() noexcept
{
	// disconnect from the muted signal since it doesn't die with us
	mutedSignal.disconnect(mutedId);

	// Note: It's important to stop before audioOutput is destroyed. If not,
	// the sound stream focus will be released when soundStream is destroyed
	// and if that happens after audioOutput has been destroyed our call to
	// audioOutput.unprepare() will crash the application.
	try { stop(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

bool Kystsoft::ValueAudio::muted = true;
Kystsoft::Signal<> Kystsoft::ValueAudio::mutedSignal;

void Kystsoft::ValueAudio::setMuted(bool muted)
{
	if (muted)
		mute();
	else
		unmute();
}

void Kystsoft::ValueAudio::mute()
{
	muted = true;
	mutedSignal.emit();
}

void Kystsoft::ValueAudio::toggleMuteUnmute()
{
	if (muted)
		unmute();
	else
		mute();
}

void Kystsoft::ValueAudio::setStarted(bool started)
{
	if (started)
		start();
	else
		stop();
}

void Kystsoft::ValueAudio::toggleStartStop()
{
	if (isStarted())
		stop();
	else
		start();
}

void Kystsoft::ValueAudio::load(const Settings& settings, const std::string& section)
{
	sound.load(settings, section);
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
}

void Kystsoft::ValueAudio::onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested)
{
	// get audio characteristics
	int sampleRate = audioOutput.sampleRate();
	audio_channel_e channel = audioOutput.channel();
	audio_sample_type_e sampleType = audioOutput.sampleType();

	// get sound characteristics
	double value = averageValue;
	double frequency = sound.frequency(value);
	double period = sound.period(value);
	double duty = sound.duty(value);

	// create silence points
	if (muted || frequency <= 0 || period <= 0 || duty <= 0)
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
	double cyclePhase = 0;
	double tonePhase = 0;
	for (size_t i = 0; i < pointCount; ++i)
	{
		cyclePhase = lastCyclePhase + double(i) / cyclePointCount;
		if (cyclePhase >= 1)
			cyclePhase -= int(cyclePhase); // keep below 1
		if (cyclePhase < duty)
		{
			double x = lastTonePhase + i * frequency / sampleRate;
			double y = 0;
			switch (sound.waveform())
			{
			case Waveform::Sine: // sine wave, https://en.wikipedia.org/wiki/Sine_wave
				y = std::sin(2 * 3.14159265358979 * x);
				break;
			case Waveform::Square: // square wave, https://en.wikipedia.org/wiki/Square_wave
				y = 2 * (2 * std::floor(x) - std::floor(2 * x)) + 1;
				break;
			case Waveform::Triangle: // triangle wave, https://en.wikipedia.org/wiki/Triangle_wave
				y = 2 * std::abs(2 * (x + 0.25 - std::floor(x + 0.75))) - 1;
				break;
			case Waveform::Sawtooth: // sawtooth wave, https://en.wikipedia.org/wiki/Sawtooth_wave
				y = 2 * (x - std::floor(x + 0.5));
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
