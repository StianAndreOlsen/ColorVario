#ifndef KYSTSOFT_SOUNDMANAGER_H
#define KYSTSOFT_SOUNDMANAGER_H

#include <sound_manager.h>

namespace Kystsoft {

class SoundManager
{
public:
	SoundManager(sound_type_e type) : soundType(type) {}
	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator=(const SoundManager& rhs) = delete;
	~SoundManager() noexcept;
	double volume() const;
	void setVolume(double volume);
private:
	int maxVolume() const;
	sound_type_e soundType = SOUND_TYPE_SYSTEM;
	double initialVolume = -1;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SOUNDMANAGER_H
