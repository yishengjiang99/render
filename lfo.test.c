#include <assert.h>
#include "lfo.c"
#include <stdlib.h>
#include <math.h>
#include "sf2.c"
#include "call_ffp.c"

int main()
{
	zone_t z;
	z.ModLFOFreq = 80;
	z.ModEnv2FilterFc = 100;
	lfo_t *l = newModLFO(z);
	assert(l->freq - powf(2.0f, (80.0 - 6900) / 1200.0f) * 440 < 10);

	FILE *outp = wavepic("wv.png");

	int t = 0;
	while (t < 57000)
	{
		lfo_run(l);
		printf("\n%f\n", &(l->output[5]));
		fwrite(&(l->output[0]), 4, blocksize, outp);
		t += blocksize;
	}
	system("open wv.png");
}