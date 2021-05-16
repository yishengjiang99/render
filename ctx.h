typedef struct
{
	int program_number;
	float midi_gain_cb;
	float midi_pan;
} channel_t;

typedef struct _ctx
{
	int sampleRate;
	uint16_t currentFrame;
	int samples_per_frame;
	int refcnt;
	float mastVol;
	channel_t channels[16];
	voice *voices;
	float *outputbuffer;
	FILE *outputFD;
} ctx_t;