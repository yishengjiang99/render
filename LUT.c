#ifndef LUT_C
#define LUT_C
#include "luts.c"
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
static inline uint32_t timecent2steps(short tc, uint32_t samplerate)
{
	return p2over1200(tc) * samplerate;
}
static inline float centtone2freq(short ct)
{
	return p2over1200(ct - 6900) * 440.0f;
}

#endif