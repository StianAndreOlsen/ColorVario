#ifndef KYSTSOFT_CLIMBAUDIO_H
#define KYSTSOFT_CLIMBAUDIO_H

#include "ValueAudio.h"

namespace Kystsoft {

class ClimbAudio : public ValueAudio
{
public:
	void load(const Settings& settings) override { ValueAudio::load(settings, "ClimbSound"); }
	void setClimb(double climb) { setValue(climb); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBAUDIO_H
