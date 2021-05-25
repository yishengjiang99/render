#include "delay.c"
#include <math.h>
#include <stdio.h>

#include "libs/wavetable-oscillator.c"
#include <assert.h>
int main()
{
	init_oscillators();
	printf("%p", oscillator[NUM_OSCILLATORS - 1].wave000);
	assert(oscillator[NUM_OSCILLATORS - 1].wave000 != NULL);
	oscillator[0].phaseIncrement = (int32_t)(BIT32_NORMALIZATION * 220 / SAMPLE_RATE + 0.5f);
	wavetable_0dimensional_oscillator(&oscillator[0]);
	assert(oscillator[0].phase > 0);
}