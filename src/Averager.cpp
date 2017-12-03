#include "Averager.h"

void Kystsoft::Averager::add(float value)
{
	v[i] = value;
	if (++i >= v.size())
		i = 0;
}
