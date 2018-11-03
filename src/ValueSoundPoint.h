#ifndef KYSTSOFT_VALUESOUNDPOINT_H
#define KYSTSOFT_VALUESOUNDPOINT_H

#include <istream>
#include <sstream>
#include <string>

namespace Kystsoft {

struct ValueSoundPoint
{
	double value = 0; // controlling value
	double frequency = 0; // tone frequency [Hz]
	double period = 0; // cycle period (sound + silence) [s]
	double duty = 0; // sound period / cycle period [-]
	ValueSoundPoint() {}
	ValueSoundPoint(double v, double f, double p, double d)
		: value(v), frequency(f), period(p), duty(d) {}
	bool load(const std::string& str) { std::istringstream is(str); return load(is); }
	bool load(std::istream& is);
};

} // namespace Kystsoft

#endif // KYSTSOFT_VALUESOUNDPOINT_H
