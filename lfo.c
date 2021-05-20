#ifndef LFO_C
#define LFO_C
#include "sf2.h"
#include "stbl.c"
#include "LUT.c"
#include <stdlib.h>
#include <stdlib.h>

typedef short timecent;
typedef short centone;
typedef short centidb;
typedef struct
{
	uint32_t delay, phase;
	float freq;
	short modfreq, modvol, modpitch;
} lfo_t;
lfo_t *newModLFO(zone_t z)
{
	lfo_t *modlfo = malloc(sizeof(lfo_t));
	*modlfo = (lfo_t){
			timecent2steps(z.ModLFODelay, 48000), 0,
			centtone2freq(z.ModLFOFreq),
			z.ModLFO2Pitch,
			z.ModLFO2FilterFc,
			z.ModLFO2Vol};
	return modlfo;
}

//based on https://github.com/yishengjiang99/nco from rbj@audionation.com
#define MASK_FRACTIONAL_BITS 0x000FFFFFL
#define MASK_WAVEINDEX 0x00000FFFUL
#define WAVETABLE_SIZE 4096
#define FRACTIONAL_RATIO (float)MASK_FRACTIONAL_BITS / WAVETABLE_SIZE
#define LOG2_WAVETABLE_SIZE 12
#define BIT32_NORMALIZATION 4294967296.0f
#define PHASE_INC(freq, samprate) (int)BIT32_NORMALIZATION *freq / samprate + 0.5f
int int_sin(uint32_t *phase, float freq, int sampleRate)
{
	double v1, v2;
	float fraction, interpolatedval;
	uint32_t index = (*phase) & MASK_WAVEINDEX;
	v1 = stbl[index];
	v2 = stbl[index + 1];
	fraction = (*phase & MASK_FRACTIONAL_BITS) * FRACTIONAL_RATIO;
	interpolatedval = v1 + (v2 - v1) * fraction;
	*phase += PHASE_INC(freq, sampleRate);
	return interpolatedval;
}

#endif