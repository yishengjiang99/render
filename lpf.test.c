#include <math.h>

#include <assert.h>
#include "cosine.c"
#include "call_ffp.c"
#include "sf2.c"
#include "libs/wavetable-oscillator.c"
int main()
{
	short cos[4096];
	short *cs = &(cos[0]); //malloc(2 * 4096);
	float f[4096];

	init_oscillators();

	FILE *pipe = ffp(2, 48000);
	zone_t *z = get_sf(0, 0, 88, 120);
	pclose(pipe);
}
