#ifndef KYSTSOFT_VARIOSOUNDPOINT_H
#define KYSTSOFT_VARIOSOUNDPOINT_H

#include <iostream>

namespace Kystsoft {

struct VarioSoundPoint
{
	float climb = 0; // vertical speed [m/s]
	float frequency = 0; // tone frequency [Hz]
	float period = 0; // cycle period (sound + silence) [s]
	float duty = 0; // sound period / cycle period [-]
	VarioSoundPoint(float c, float f, float p, float d)
		: climb(c), frequency(f), period(p), duty(d) {}
	bool load(std::istream& is, char delimiter = '\t');
	void save(std::ostream& os, char delimiter = '\t') const;
	bool operator<(const VarioSoundPoint& other) const
		{ return climb < other.climb; }
};

} // namespace Kystsoft

#endif // KYSTSOFT_VARIOSOUNDPOINT_H
