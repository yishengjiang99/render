#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#include "libs/fft.h"
#include "sf2.h"
#include "libs/wavetable_oscillator.h"

static double *stbl;
static FILE *fdout;
void simple(complex *cs, int N)
{
	int n = log2(N);

	FFT(cs, n, stbl);
	bit_reverse(cs, n);
	printf("\noutput\n");

	for (int i = 0; i < n; i++)
	{
		printf("\n %d: %f \t %f", i, cs[i].real, cs[i].imag);
	}
}
void trucate(complex *cs, int N, int npartials)
{
	int n = log2(N);
	FFT(cs, n, stbl);
	bit_reverse(cs, n);

	for (int i = 0; i < N; i++)
	{
		if (i < npartials && i > N - npartials)
		{
			cs[i].real = 0;
			cs[i].imag = 0;
		}
	}
	bit_reverse(cs, n);
	iFFT(cs, n, stbl);
}
void spinFloat(wavetable_oscillator_data *oscref, float *ff, int midi)
{
	oscref->wave000 = ff;
	set_midi(0, midi);

	for (int i = 0; i < 48000; i += oscref->samples_per_block)
	{
		wavetable_0dimensional_oscillator(oscref);
		fwrite(oscref->output_ptr, sizeof(float), oscref->samples_per_block, fdout);
	}
}

int main()
{
	fdout = popen("ffmpeg -y -f f32le -i pipe:0 -ac 2 -ar 48000 -f flac fftp_truncate_5_partials.flac", "w");
	int N = 4096, n = log2(N);
	stbl = malloc(sizeof(double) * N / 4);
	complex c1s[N];
	bzero(c1s, N * sizeof(complex));
	float ff[N];
	fread(ff, sizeof(float), N, fopen("sawtooth.pcm", "rb"));
	wavetable_oscillator_data *oscref = init_oscillators();
	spinFloat(oscref, ff, 44);
	spinFloat(oscref, ff, 46);
	spinFloat(oscref, ff, 35);
	spinFloat(oscref, ff, 70);

	for (int i = 0; i < N; i++)
	{
		//	c1s[i].real =
		c1s[i].real = ff[i];
		c1s[i].imag = 0.0f;
	}
	sin_table(stbl, 10L);
	assert(stbl[0] < 0.0001f);
	simple(c1s, N);
	trucate(c1s, N, 5);
	for (int i = 0; i < N; i++)
	{
		//	c1s[i].real =
		ff[i] = c1s[i].real; // = ff[i];
	}
	spinFloat(oscref, ff, 44);
	spinFloat(oscref, ff, 46);
	spinFloat(oscref, ff, 35);
	spinFloat(oscref, ff, 70);
}