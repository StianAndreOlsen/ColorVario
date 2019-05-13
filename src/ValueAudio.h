#ifndef KYSTSOFT_VALUEAUDIO_H
#define KYSTSOFT_VALUEAUDIO_H

#include "AudioOutput.h"
#include "Averager.h"
#include "Signal.h"
#include "SoundStream.h"
#include "ValueSound.h"
#include <ctime>

namespace Kystsoft {

class ValueAudio
{
public:
	ValueAudio();
	ValueAudio(const ValueAudio& other) = delete;
	ValueAudio& operator=(const ValueAudio& rhs) = delete;
	virtual ~ValueAudio() noexcept;
	virtual void load(const Settings& settings) = 0;
	static bool isMuted() { return muted; }
	static void setMuted(bool muted);
	static void mute();
	static void unmute() { muted = false; }
	static void toggleMuteUnmute();
	bool isStarted() const { return audioOutput.isPrepared(); }
	void setStarted(bool started);
	void start() { if (!muted) audioOutput.prepare(); }
	void stop() { audioOutput.unprepare(); }
	void toggleStartStop();
	const ValueSound& valueSound() const { return sound; }
	void setValueSound(const ValueSound& valueSound) { sound = valueSound; }
	void setSamplingInterval(double interval) { averageValue.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	void setValue(double value) { averageValue += value; }
protected:
	void load(const Settings& settings, const std::string& section);
private:
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	static bool muted;
	static Signal<> mutedSignal;
	Signal<>::ConnectionId mutedId = 0;
	SoundStream soundStream;
	AudioOutput audioOutput;
	double lastCyclePhase = 0;
	double lastTonePhase = 0;
	ValueSound sound;
	Averager<double> averageValue;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEAUDIO_H
