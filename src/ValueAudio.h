#ifndef KYSTSOFT_VALUEAUDIO_H
#define KYSTSOFT_VALUEAUDIO_H

#include "AudioOutput.h"
#include "Averager.h"
#include "Signal.h"
#include "SoundStream.h"
#include "ValueSound.h"
#include <limits>
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
	bool isMuted() const { return !audioOutput.isPrepared(); }
	void setMuted(bool muted);
	void mute();
	void unmute();
	void toggleMuteUnmute();
	const Signal<bool>& mutedSignal() const { return mutedSignl; }
	void setValueSound(const ValueSound& valueSound) { sound = valueSound; }
	const ValueSound& valueSound() const { return sound; }
	void setSamplingInterval(double interval) { average.setSamplingInterval(interval); }
	void setAveragingInterval(double interval) { average.setAveragingInterval(interval); }
	void setValue(double value);
protected:
	void load(const Settings& settings, const std::string& section);
private:
	void onSoundStreamFocusChanged(int focus);
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	bool isSoundOn() const { return soundOn != 0; }
	bool isSoundOff() const { return soundOn == 0; }
	bool isUpperSoundOn() const { return soundOn > 0; }
	bool isLowerSoundOn() const { return soundOn < 0; }
	SoundStream soundStream;
	AudioOutput audioOutput;
	double lastCyclePhase = 0;
	double lastTonePhase = 0;
	time_t lastWriteTime = 0;
	Signal<bool> mutedSignl;
	ValueSound sound;
	Averager<double> average;
	double current = std::numeric_limits<double>::quiet_NaN();
	short soundOn = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEAUDIO_H
