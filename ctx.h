typedef struct
{
	int program_number;
	unsigned short midi_volume;
	unsigned short midi_pan;
} channel_t;
typedef struct _voice
{
	zone_t *z;
	shdrcast *sample;
	unsigned int start, end, startloop, endloop;
	uint32_t pos;
	float frac;
	float ratio;
	adsr_t *ampvol;
	int midi;
	int velocity;
	int chid;
	float panLeft, panRight;
	short attenuate;

} voice;
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