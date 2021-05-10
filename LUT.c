
#ifndef read2_h
#include "read2.h"
#endif
#include <math.h>
static float p2over1200LUT[1200];
static inline float p2over1200(float x)
{
	if (x < -12000)
		return 0;
	if (x < 0)
		return 1.f / p2over1200(-x);
	else if (x > 1200.0f)
	{
		return 2 * p2over1200(x - 1200.0f);
	}
	else
	{
		return p2over1200LUT[(unsigned short)(x)];
	}
}
static float centdbLUT[960];
static float centdblut(int x)
{
	if (x < 0)
		x = 0;
	if (x > 960)
		x = 960;

	return centdbLUT[x];
}
void initLUTs()
{
	for (int i = 0; i < 2400; i++)
	{
		p2over1200LUT[i] = powf(2.0f, i / 1200.0f);
	}
	for (int i = 0; i < 960; i++)
	{
		centdbLUT[i] = powf(10.0f, i / -200.0);
	}
}
