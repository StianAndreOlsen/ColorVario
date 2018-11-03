#include "Geoid.h"
#include "algorithm.h"
#include "dlog.h"
#include <cmath>
#include <fstream>

bool Kystsoft::Geoid::load(const std::string& fileName)
{
	std::ifstream is(fileName);
	if (!is.is_open() || !load(is))
	{
		dlog(DLOG_ERROR) << "Unable to load Geoid data from \"" << fileName << "\"";
		return false;
	}
//	dlog(DLOG_DEBUG) << "Geoid data (" << ni << " x " << nj << ") loaded from \"" << fileName << "\"";
	return true;
}

bool Kystsoft::Geoid::load(std::istream& is)
{
	if (!(is >> ni >> nj))
	{
		ni = nj = 0;
		heights.clear();
		return false;
	}
	heights.resize(ni * nj, 0);
	for (auto& height : heights)
		if (!(is >> height))
			return false;
	return true;
}

// This algorithm and the Geoid data are derived from the following web page:
// http://www.open-terrain.org/index.php/Projects/EGM08 (OpenTerrain Earth Geodetic Model)
double Kystsoft::Geoid::height(double latitude, double longitude) const
{
	if (ni < 2 || nj < 2)
		return 0;

	// normalize coordinates
	double x = (longitude - -0) / (360 - -0);
	double y = (latitude - -90) / (90 - -90);
	x = x - std::floor(x);
	y = std::clamp(y, 0.0, 1.0);

	// find grid cell and cell x and y
	x *= ni - 1;
	y *= nj - 1;
	size_t i = size_t(x + 0.5);
	size_t j = size_t(y + 0.5);
	x -= i;
	y -= j;
	if (i == ni - 1)
	{
		i = ni - 2;
		x = 1;
	}
	if (j == nj - 1)
	{
		j = nj - 2;
		y = 1;
	}

	// interpolate using bilinear interpolation
	// https://en.wikipedia.org/wiki/Bilinear_interpolation
	double h1 = height(i  , j  );
	double h2 = height(i+1, j  );
	double h3 = height(i  , j+1);
	double h4 = height(i+1, j+1);
	return (1 - y) * ((1 - x) * h1 + x * h2) +
	            y  * ((1 - x) * h3 + x * h4);
}
