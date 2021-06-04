
#ifndef LFO_C
#define LFO_C
#include "runtime.c"
typedef short timecent;
typedef short centone;
typedef short centidb;
#define blocksize 128
typedef struct
{
	uint32_t delay, phase;
	int32_t phaseInc;
	float val, stepIncVal;
	short modfreq, modvol, modpitch;
	float output[blocksize];
} lfo_t;

void initLFO(lfo_t *lfo, short delayct, short freqct)
{
	lfo->delay = timecent2steps(delayct, output_sampleRate) / dspbuffersize;
	lfo->stepIncVal = (float)(4.f * centtone2freq(freqct) / output_sampleRate * dspbuffersize);
	lfo->val = 0.0f;
}
void runlfo(lfo_t *lfo)
{
	if (lfo->delay > 0)
		lfo->delay--;
	else
	{
		lfo->val += lfo->stepIncVal;
		if (lfo->val > 1.0f)
		{
			lfo->val = 1.0f - (lfo->val - 1.0f);
			lfo->stepIncVal *= -1;
		}
		if (lfo->val < -1.0f)
		{
			lfo->val = -2.0f + lfo->val;
			lfo->stepIncVal *= -1;
		}
		printf("%f", lfo->val);
	}
}
float lfo_mod_vol(float val, short modvol)
{
	return centdblut(val * modvol);
}

float lfo_mod_pitch(float val, short modpitch)
{
	return ct2relativePitch(val * modpitch);
}
float lfo_mod_filter(float val, short modFC)
{
	return ct2relativePitch(val * modFC);
}
#endif
