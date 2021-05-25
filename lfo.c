
#ifndef LFO_C
#define LFO_C
#include "sf2.h"
#include "stbl.c"
#include "LUT.c"
#include <stdlib.h>

typedef short timecent;
typedef short centone;
typedef short centidb;
#define blocksize 128
typedef struct
{
	uint32_t delay, phase;
	int32_t phaseInc;
	short modfreq, modvol, modpitch;
	float output[blocksize];
} lfo_t;

//based on https://github.com/yishengjiang99/nco from rbj@audionation.com
#define MASK_FRACTIONAL_BITS 0x000FFFFFL
#define MASK_WAVEINDEX 0x00000FFFUL
#define WAVETABLE_SIZE 4096
#define FRACTIONAL_RATIO (float)MASK_FRACTIONAL_BITS / WAVETABLE_SIZE
#define LOG2_WAVETABLE_SIZE 12
#define BIT32_NORMALIZATION 4294967296.0f
#define PHASE_INC(freq, samprate) (int)(BIT32_NORMALIZATION * freq / samprate + 0.5f)
float int_sin(uint32_t *phase, int32_t phaseInc)
{
	double v1, v2;
	float fraction, interpolatedval;
	uint32_t index = (*phase) & MASK_WAVEINDEX;
	v1 = stbl[index];
	v2 = stbl[index + 1];
	fraction = (*phase & MASK_FRACTIONAL_BITS) * FRACTIONAL_RATIO;
	interpolatedval = v1 + (v2 - v1) * fraction;
	*phase += phaseInc;
	return interpolatedval;
}
void lfo_run(lfo_t *lfo)
{
	int todo = blocksize;

	while (todo-- > 0)
	{
		if (lfo->delay-- > 0)
		{
			lfo->output[blocksize - todo] = 0;
		}
		else
		{
			lfo->output[blocksize - todo] = int_sin(&lfo->phase, lfo->phaseInc); //, 48000);
		}
	}
}
#endif
lfo_t *newLFO(float freq)
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
