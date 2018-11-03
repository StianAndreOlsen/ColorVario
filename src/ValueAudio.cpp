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
	soundStream.focusChangedSignal().connect(this, &ValueAudio::onSoundStreamFocusChanged);
	audioOutput.setSoundStreamInfo(soundStream);
	audioOutput.writeCallback().connect(this, &ValueAudio::onAudioRequested);
}

Kystsoft::ValueAudio::~ValueAudio() noexcept
{
	// Note: It is important to mute before audioOutput is destroyed. If not,
	// the sound stream focus will be released when soundStream is destroyed
	// and if that happens after audioOutput has been destroyed our call to
	// audioOutput.unprepare() will crash the application.
	try { mute(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::ValueAudio::setMuted(bool muted)
{
	if (muted)
		mute();
	else
		unmute();
}

void Kystsoft::ValueAudio::mute()
{
	if (isMuted())
		return;
	soundStream.releasePlaybackFocus();
}

void Kystsoft::ValueAudio::unmute()
{
	if (!isMuted())
		return;
	soundStream.acquirePlaybackFocus();
}

void Kystsoft::ValueAudio::toggleMuteUnmute()
{
	if (isMuted())
		unmute();
	else
		mute();
}

void Kystsoft::ValueAudio::setValue(double value)
{
	double previous = current;
	average += value;
	current = average;
	if (current == previous)
		return;

	// sound on or off
	if (std::isnan(previous))
	{
		// special handling if this is the first real value
		if (current >= sound.upperSoundOnThreshold())
			soundOn = 1; // turn on upper sound
		else if (current <= sound.lowerSoundOnThreshold())
			soundOn = -1; // turn on lower sound
	}
	else if (current > previous)
	{
		if (isLowerSoundOn() && current >= sound.lowerSoundOffThreshold())
			soundOn = 0; // turn off lower sound
		if (isSoundOff() && current >= sound.upperSoundOnThreshold())
			soundOn = 1; // turn on upper sound
	}
	else // remember that equality is tested at function start
	{
		if (isUpperSoundOn() && current <= sound.upperSoundOffThreshold())
			soundOn = 0; // turn off upper sound
		if (isSoundOff() && current <= sound.lowerSoundOnThreshold())
			soundOn = -1; // turn on lower sound
	}

	// TODO: Remove if this never happens (check log file after a long period of testing)
	// restart audio if it has stopped (for some mysterious reason)
	if (!isMuted() && lastWriteTime > 0 && std::difftime(std::time(nullptr), lastWriteTime) > 2)
	{
		lastWriteTime = 0;
		audioOutput.unprepare();
		audioOutput.prepare();
		dlog(DLOG_INFO) << "Audio is restarted!";
	}
}

void Kystsoft::ValueAudio::load(const Settings& settings, const std::string& section)
{
	setMuted(settings.value("Sound.muted", false));
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
	sound.load(settings, section);
}

void Kystsoft::ValueAudio::onSoundStreamFocusChanged(int focus)
{
	lastWriteTime = 0;
	if (focus & SOUND_STREAM_FOCUS_FOR_PLAYBACK)
	{
		// focus gained --> start audio
		audioOutput.prepare();
		mutedSignl.emit(false);
	}
	else
	{
		// focus lost --> stop audio
		audioOutput.unprepare();
		mutedSignl.emit(true);
	}
}

void Kystsoft::ValueAudio::onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested)
{
	// save the last write time
	lastWriteTime = std::time(nullptr);

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
	double frequency = sound.frequency(current);
	double period = sound.period(current);
	double duty = sound.duty(current);

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
			case Waveform::Sine: // sine wave (https://en.wikipedia.org/wiki/Sine_wave)
				y = std::sin(2 * 3.14159265358979 * x);
				break;
			case Waveform::Square: // square wave (https://en.wikipedia.org/wiki/Square_wave)
				y = 2 * (2 * std::floor(x) - std::floor(2 * x)) + 1;
				break;
			case Waveform::Triangle: // triangle wave (https://en.wikipedia.org/wiki/Triangle_wave)
				y = 2 * std::abs(2 * (x + 0.25 - std::floor(x + 0.75))) - 1;
				break;
			case Waveform::Sawtooth: // sawtooth wave (https://en.wikipedia.org/wiki/Sawtooth_wave)
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
