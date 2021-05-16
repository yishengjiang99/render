#include <math.h>

#include <assert.h>
#include "cosine.c"
#include "libs/oscwave.c"
#include "call_ffp.c"
int main()
{
	short cos[4096];
	short *cs = &(cos[0]); //malloc(2 * 4096);
	float f[4096];

	init_oscillators();
	oscillator[0].phaseIncrement = (int32_t)(1 + 440.0f * BIT32_NORMALIZATION / SAMPLE_RATE);
	FILE *pipe = ffp(1, 48000);
	set_midi(0, 60);

	set_midi(1, 90);

	oscillator[1].output_ptr = oscillator[0].output_ptr;
	for (int i = 0; i < 49000; i += SAMPLE_BLOCKSIZE)
	{
		// if (i % 10000 == 5000)
		// 	oscillator[0].frequencyIncrement = 1.0f; // / SAMPLE_RATE;
		// else if (i * 10000 == 8000)
		// {
		// 	oscillator[0].frequencyIncrement = -1.0f; // / SAMPLE_RATE;
		// }
		bzero(oscillator[1].output_ptr, SAMPLE_BLOCKSIZE);
		wavetable_1dimensional_oscillator(&oscillator[0]);
		wavetable_1dimensional_oscillator(&oscillator[1]);

		fwrite(&oscillator[0].output_ptr, 4, SAMPLE_BLOCKSIZE, pipe);
	}
	pclose(pipe);
}
