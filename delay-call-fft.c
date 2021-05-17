#include "delay.c"
#include <math.h>

#include <stdio.h>

#include "libs/wavetable-oscillator.c"
#include <assert.h>
#include "libs/fft.c"
#include "stbl.c"

void input_time_domain_floats(int n, float *reals, complex *x, double *stbl)
{
	complex *ptr = x;
	while (n-- > 0)
	{
		ptr->real = *reals;
		ptr->imag = 0.0f;
		ptr++;
	}
	FFT(x, 12L, stbl);
}
int main()
{

	init_oscillators();
	//printf("%p", oscillator[NUM_OSCILLATORS - 1].wave000);
	assert(oscillator[NUM_OSCILLATORS - 1].wave000 != NULL);
	oscillator[0].phaseIncrement = (int32_t)(BIT32_NORMALIZATION * 220 / SAMPLE_RATE + 0.5f);
	delay_t *dl = newDelay(4096 * 4, 4096);
	complex *cs = (complex *)malloc(sizeof(complex) * 4096);
	float *fftinput = malloc(4096 * sizeof(float));
	for (int i = 0; i < 58000; i += 128)
	{
		wavetable_0dimensional_oscillator(&oscillator[0]);
		dl_write(dl, 128, oscillator[0].output_ptr);
		input_time_domain_floats(4096, dl->read, cs, &(stbl[0]));
	}

	// delay_snds(dl);

	assert(oscillator[0].phase > 0);
}
