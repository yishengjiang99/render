#ifndef LPF_C
#define LPF_C
#include <math.h>
#include <string.h>
typedef struct
{
	/*
   X = exp(-2.0 * pi * Fc)
 *     A = 1 - X
 *     B = X
 *     Fc = cutoff freq / sample rate*/
	float cutoff_freq;
	int sample_rate;
	float input;
	float output;
	float m1;
	float X;
} lpf_t;

#define pi 3.1415f

lpf_t *newLpf(lpf_t *l, float cutoff_freq, float sample_rate)
{
	l->sample_rate = sample_rate;
	l->cutoff_freq = cutoff_freq;
	l->m1 = 0;
	l->X = expf(-2.0f * pi * (float)(l->cutoff_freq / l->sample_rate));
	return l;
}

float process_input(lpf_t *l, float input)
{
	l->input = input;
	l->output = l->input * (1.0f - l->X) + l->m1 * l->X;
	//printf("\n%f ", l->input * (1.0f - l->X));
	l->m1 = l->input * (1.0f - l->X) + l->m1 * l->X;
	return l->m1;
}

#endif