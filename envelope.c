#ifndef read2_h
#include "sf2.h"
#endif
#ifndef LUT
#include "LUT.c"
#endif
#include <math.h>
#include <stdlib.h>

#define adsr 1
#define fmax(a, b) a > b ? a : b
typedef struct
{
	uint32_t att_steps, decay_steps, release_steps, delay_steps, hold_steps;
	unsigned short sustain;
	float db_attenuate;
	float att_rate, decay_rate, release_rate;
} adsr_t;

adsr_t *newEnvelope(short centAtt, short centRelease, short centDecay, short sustain, int sampleRate)
{
	adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
	env->att_steps = fmax(p2over1200(centAtt) * sampleRate, 58);
	env->decay_steps = fmax(p2over1200(centDecay) * sampleRate, 2);
	env->release_steps = fmax(p2over1200(centRelease) * sampleRate, 2);
	env->att_rate = -960.0f / env->att_steps;
	env->decay_rate = ((float)1.0f * sustain) / (env->decay_steps);
	env->release_rate = (float)(1000 - sustain) / (env->release_steps);
	env->db_attenuate = 960.0f;
	return env;
}
float envShift(adsr_t *env)
{
	if (env->att_steps > 0)
	{
		env->att_steps--;
		env->db_attenuate += env->att_rate;
	}
	else if (env->decay_steps > 0)
	{
		env->decay_steps--;
		env->db_attenuate += env->decay_rate;
	}
	else if (env->release_steps > 0)
	{
		env->release_steps--;
		env->db_attenuate += env->release_rate;
	}
	if (env->db_attenuate > 960)
	{
		env->db_attenuate = 960.0f;
	}
	if (env->db_attenuate < 0.0)
	{
		env->db_attenuate = 0.0f;
	}
	return env->db_attenuate;
}
void adsrRelease(adsr_t *env)
{
	env->decay_steps = 0;
	env->att_steps = 0;
}