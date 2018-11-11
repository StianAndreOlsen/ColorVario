#include "ValueColorPoint.h"

bool Kystsoft::ValueColorPoint::load(std::istream& is)
{
	float r = 0, g = 0, b = 0, a = 0;
	if (is >> value >> r >> g >> b >> a)
	{
		if (r > 1.5f || g > 1.5f || b > 1.5f || a > 1.5f)
		{
			int R = int(r + 0.5f);
			int G = int(g + 0.5f);
			int B = int(b + 0.5f);
			int A = int(a + 0.5f);
			color = Color(R,G,B,A);
			return true;
		}
		color = Color(r,g,b,a);
		return true;
	}
	return false;
}
