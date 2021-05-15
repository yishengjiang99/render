
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
} lpf;

#define pi 3.1415f

lpf *newLpf(lpf *l, float cutoff_freq, int sample_rate)
{

	l->cutoff_freq = cutoff_freq;
	l->m1 = 0;
	l->X = expf(-2.0f * pi * (float)l->cutoff_freq / sample_rate);
	return l;
}

void process(lpf *l)
{
	l->output = l->input * (1 - l->X) + l->m1 * l->X;
	l->m1 = l->output;
	memcpy(&l->m1, &l->output, 4);
}
float process_input(lpf *l, float input)
{
	l->input = input;
	l->output = l->input * (1.0f - l->X) + l->m1 * l->X;
	//printf("\n%f ", l->input * (1.0f - l->X));
	l->m1 = l->input * (1.0f - l->X) + l->m1 * l->X;
	return l->m1;
}