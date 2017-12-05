#ifndef KYSTSOFT_VARIOSOUND_H
#define KYSTSOFT_VARIOSOUND_H

#include "AudioOutput.h"
#include "VarioSoundSettings.h"

namespace Kystsoft {

class VarioSound
{
public:
	VarioSound();
	VarioSound(const VarioSound& other) = delete;
	VarioSound& operator=(const VarioSound& rhs) = delete;
	bool isMuted() const { return muted; }
	void mute();
	void unmute();
	void toggleMuteUnmute();
	void setSettings(const VarioSoundSettings& settings) { sound = settings; }
	const VarioSoundSettings& settings() const { return sound; }
	void setClimb(float climb);
private:
	bool isSoundOn() const { return soundOn != 0; }
	bool isSoundOff() const { return soundOn == 0; }
	bool isClimbSoundOn() const { return soundOn > 0; }
	bool isSinkSoundOn() const { return soundOn < 0; }
	void turnAudioOn();
	void turnAudioOff();
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	AudioOutput audioOutput;
	float lastCyclePhase = 0;
	float lastTonePhase = 0;
	bool muted = true;
	VarioSoundSettings sound;
	float currentClimb = 10; // start with a value...
	short soundOn = 1; // ...that has climb sound
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOSOUND_H
