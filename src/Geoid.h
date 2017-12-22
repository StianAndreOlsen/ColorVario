#ifndef KYSTSOFT_GEOID_H
#define KYSTSOFT_GEOID_H

#include <istream>
#include <string>
#include <vector>

namespace Kystsoft {

class Geoid
{
public:
	bool load(const std::string& fileName);
	bool load(std::istream& is);
	double height(double latitude, double longitude) const; // geoid height above reference ellipsoid
private:
	double height(size_t i, size_t j) const { return heights[i + j * ni]; }
	size_t ni = 0; // number of heights in x-direction
	size_t nj = 0; // number of heights in y-direction
	std::vector<float> heights; // use floats to save memory
};

} // namespace Kystsoft

#endif // KYSTSOFT_GEOID_H
