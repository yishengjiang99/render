
#ifndef LFO_C
#define LFO_C
#include "ctx.h"
#include "sf2.h"
#include "LUT.c"

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
<<<<<<< HEAD
			lfo->output[blocksize - todo] = int_sin(&lfo->phase, lfo->phaseInc); //, 48000);
=======
			lfo->val = -2.0f + lfo->val;
			lfo->stepIncVal *= -1;
>>>>>>> master
		}
		printf("%f", lfo->val);
	}
}
<<<<<<< HEAD
#endif lfo_t *newLFO(float freq)
{
	lfo_t *lfo = (lfo_t *)malloc(sizeof(lfo_t));
	lfo->phaseInc = PHASE_INC(freq, 48000);
	return lfo;
}
lfo_t *newModLFO(zone_t z)
{
	lfo_t *lfo = newLFO(centtone2freq(z.ModLFOFreq));
	lfo->delay = timecent2steps(z.ModLFODelay, 48000);
	lfo->modfreq = z.ModEnv2FilterFc;
	return lfo;
}
=======
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
>>>>>>> master
