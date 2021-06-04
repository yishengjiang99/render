typedef struct
{
	int sample_rate, cutoff_freq;
	float m1, X, input, output;
} lpf_t;
lpf_t *newLpf(lpf_t *l, float cutoff_freq, float sample_rate);
float process_input(lpf_t *l, float input);