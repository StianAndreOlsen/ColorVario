#ifndef KYSTSOFT_ALTITUDEAUDIO_H
#define KYSTSOFT_ALTITUDEAUDIO_H

#include "ValueAudio.h"

namespace Kystsoft {

class AltitudeAudio : public ValueAudio
{
public:
	void load(const Settings& settings) override { ValueAudio::load(settings, "AltitudeSound"); }
	void setAltitude(double altitude) { setValue(altitude); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEAUDIO_H
