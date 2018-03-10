#ifndef KYSTSOFT_VARIOAUDIO_H
#define KYSTSOFT_VARIOAUDIO_H

#include "AudioOutput.h"
#include "Signal.h"
#include "VarioSound.h"
#include <ctime>

namespace Kystsoft {

class VarioAudio
{
public:
	VarioAudio();
	VarioAudio(const VarioAudio& other) = delete;
	VarioAudio& operator=(const VarioAudio& rhs) = delete;
	void load(const Settings& settings);
	bool isMuted() const { return !audioOutput.isPrepared(); }
	void setMuted(bool muted);
	void mute();
	void unmute();
	void toggleMuteUnmute();
	const Signal<bool>& mutedSignal() const { return mutedSignl; }
	void setVarioSound(const VarioSound& varioSound) { sound = varioSound; }
	const VarioSound& varioSound() const { return sound; }
	void setClimb(float climb);
private:
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	bool isSoundOn() const { return soundOn != 0; }
	bool isSoundOff() const { return soundOn == 0; }
	bool isClimbSoundOn() const { return soundOn > 0; }
	bool isSinkSoundOn() const { return soundOn < 0; }
	AudioOutput audioOutput;
	float lastCyclePhase = 0;
	float lastTonePhase = 0;
	time_t lastWriteTime = 0;
	Signal<bool> mutedSignl;
	VarioSound sound;
	float currentClimb = 3.1e+8f; // start with an unrealistic value (higher than the speed of light)
	short soundOn = 0;
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOAUDIO_H
