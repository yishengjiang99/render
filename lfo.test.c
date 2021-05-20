#include <assert.h>
#include "lfo.c"
#include <stdlib.h>
#include <stdlib.h>

int main()
{
	zone_t z;
	z.ModLFOFreq = 80;
	z.ModEnv2FilterFc = 100;
	lfo_t *l = newModLFO(z);
	assert(l->freq - powf(2.0f, (80.0 - 6900) / 1200.0f) * 440 < 10);
	z.ModLFOFreq = 6900;
}