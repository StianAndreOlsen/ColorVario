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
	bool isStarted() const { return soundStream.hasPlaybackFocus(); }
	void setStarted(bool started);
	void start();
	void stop() { soundStream.releasePlaybackFocus(); }
	void toggleStartStop();
	bool isStartedOrStopped() const; // returns true if audio is started or stopped since the last call to this function
	const ValueSound& valueSound() const { return sound; }
	void setValueSound(const ValueSound& valueSound) { sound = valueSound; }
	void setSamplingInterval(double interval) { averageValue.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { averageValue.setAveragingInterval(interval); }
	void setValue(double value);
protected:
	void load(const Settings& settings, const std::string& section);
private:
	void onSoundStreamFocusChanged(int focus);
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	static bool muted;
	static Signal<> mutedSignal;
	mutable bool startedOrStopped = false;
	Signal<>::ConnectionId mutedId = 0;
	SoundStream soundStream;
	AudioOutput audioOutput;
	double lastCyclePhase = 0;
	double lastTonePhase = 0;
	time_t lastWriteTime = 0;
	ValueSound sound;
	Averager<double> averageValue;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEAUDIO_H
