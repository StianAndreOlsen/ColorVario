#ifndef KYSTSOFT_VARIOAUDIO_H
#define KYSTSOFT_VARIOAUDIO_H

#include "AudioOutput.h"
#include "VarioSound.h"

namespace Kystsoft {

class VarioAudio
{
public:
	VarioAudio();
	VarioAudio(const VarioAudio& other) = delete;
	VarioAudio& operator=(const VarioAudio& rhs) = delete;
	bool isMuted() const { return muted; }
	void mute();
	void unmute();
	void toggleMuteUnmute();
	void setSoundSettings(const VarioSound& settings) { sound = settings; }
	const VarioSound& soundSettings() const { return sound; }
	void setClimb(float climb);
private:
	void turnAudioOn();
	void turnAudioOff();
	void onAudioRequested(AudioOutput& audioOutput, size_t bytesRequested);
	bool isSoundOn() const { return soundOn != 0; }
	bool isSoundOff() const { return soundOn == 0; }
	bool isClimbSoundOn() const { return soundOn > 0; }
	bool isSinkSoundOn() const { return soundOn < 0; }
	AudioOutput audioOutput;
	float lastCyclePhase = 0;
	float lastTonePhase = 0;
	bool muted = true;
	VarioSound sound;
	float currentClimb = 10; // start with a value...
	short soundOn = 1; // ...that has climb sound
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOAUDIO_H
