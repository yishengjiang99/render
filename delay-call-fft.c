#include "delay.c"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "libs/fft.c"
#include "stbl.c"
#include "sf2.c"
#include "runtime.c"
static FILE *fftrecod, *ifftr;
void input_time_domain_floats(int n, float *reals, complex *x, double *stbl)
{
	printf("fftc");
	complex *ptr = x;
	while (n-- > 0)
	{
		ptr->real = *reals++;
		ptr->imag = *reals++;
		ptr++;
	}
	FFT(x, 12L, stbl);
	complex *cpy = (complex *)malloc(sizeof(x));
	memcpy(cpy, x, sizeof(complex) * 4096);
	bit_reverse(cpy, 4096);
	for (int i = 0; i < 4096; i++)
	{
		fprintf(fftrecod, "\n%f,%f", cpy->real, cpy->imag);
		cpy++;
	}
	iFFT(x, 12L, stbl);
	for (int i = 0; i < 4096; i++)
	{
		fprintf(ifftr, "\n%f,%f", x->real, x->imag);
		x++;
	}
}
float *complex_gt_real_floats(complex *cs, int n)
{
	float *f = cs;
	while (n--)
	{
		*f = (float)cs->real;
		cs++;
		f++;
	}
	return f;
}
int main()
{

	delay_t *dl = newDelay(4096, 1024);
	complex *cs = (complex *)malloc(sizeof(complex) * 4096);
	init_ctx();

	g_ctx->outputFD = wavepic("wave.png");
	fftrecod = fopen("fftrecod.txt", "w");
	ifftr = fopen("ifftr.txt", "w");
	FILE *after = wavepic("waveafter.png");
	int ffttook = 0;

	for (int i = 0; i < 58000; i += 128)
	{
		render(g_ctx);
		dl_write(dl, 128, g_ctx->outputbuffer);

		if (ffttook <= i / 12000.0f)
		{
			input_time_domain_floats(4096, dl->buffer, cs, &(stbl[0]));
			ffttook++;
		}
		fwrite(complex_gt_real_floats(cs, 4096), 4096, 4, after);
	}
	pclose(g_ctx->outputFD);

	fclose(after);
	system(
			"open waveafter.png");
	//assert(oscillator[0].phase > 0);
}
