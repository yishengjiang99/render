#include <stdio.h>
#include <math.h>
#ifndef p2over1200LUT
#include "luts.c"

#endif
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
static float centdblut(int x)
{
	if (x < 0)
		x = 0;
	if (x > 960)
		x = 960;

	return centdbLUT[x];
}
static float midiCBlut(int midi)
{
	if (midi < 0)
		return -960.0f;
	if (midi > 960)
		return 0.0f;
	return midiCB[midi];
}