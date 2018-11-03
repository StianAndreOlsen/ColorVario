#include "SoundManager.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::SoundManager::~SoundManager() noexcept
{
	try { setVolume(initialVolume); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

double Kystsoft::SoundManager::volume() const
{
	int ivolume = 0;
	int error = sound_manager_get_volume(soundType, &ivolume);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_get_volume", error);
	return double(ivolume) / maxVolume();
}

void Kystsoft::SoundManager::setVolume(double volume)
{
	if (volume < 0)
		return;
	if (volume > 1)
		volume = 1;
	if (initialVolume < 0)
		initialVolume = this->volume();
	int ivolume = int(volume * maxVolume() + 0.5);
	int error = sound_manager_set_volume(soundType, ivolume);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_set_volume", error);
}

int Kystsoft::SoundManager::maxVolume() const
{
	int maxVolume = 0;
	int error = sound_manager_get_max_volume(soundType, &maxVolume);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_get_max_volume", error);
	return maxVolume;
}
