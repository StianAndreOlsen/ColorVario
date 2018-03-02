#ifndef KYSTSOFT_SOUNDMANAGER_H
#define KYSTSOFT_SOUNDMANAGER_H

#include <sound_manager.h>

namespace Kystsoft {

class SoundManager
{
public:
	SoundManager(sound_type_e type) : soundType(type) {}
	~SoundManager() noexcept;
	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator=(const SoundManager& rhs) = delete;
	float volume() const;
	void setVolume(float volume);
private:
	int maxVolume() const;
	sound_type_e soundType = SOUND_TYPE_SYSTEM;
	float initialVolume = -1;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SOUNDMANAGER_H
