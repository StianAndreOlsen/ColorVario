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
	bool isMuted() const { return !audioOutput.isPrepared(); }
	void setMuted(bool muted);
	void mute();
	void unmute();
	void toggleMuteUnmute();
	const Signal<bool>& mutedSignal() const { return mutedSignl; }
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
	SoundStream soundStream;
	AudioOutput audioOutput;
	double lastCyclePhase = 0;
	double lastTonePhase = 0;
	time_t lastWriteTime = 0;
	Signal<bool> mutedSignl;
	Averager<double> averageValue;
	ValueSound sound;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUEAUDIO_H
