#include "SampleAverager.h"

void Kystsoft::SampleAverager::add(float value)
{
	v[i] = value;
	if (++i >= v.size())
		i = 0;
}
