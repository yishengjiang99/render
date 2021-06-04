#include "sf2.h"
#define output_sampleRate 48000
#define dspbuffersize 128
typedef struct
{
	uint32_t att_steps, decay_steps, release_steps;
	unsigned short sustain;
	float db_attenuate;
	float att_rate, decay_rate, release_rate;
} adsr_t;
typedef struct
{
	int sample_rate, cutoff_freq;
	float m1, X, input, output;
} lpf_t;
lpf_t *newLpf(lpf_t *l, float cutoff_freq, float sample_rate);
float process_input(lpf_t *l, float input);
typedef struct _voice
{
	zone_t *z;
	shdrcast *sample;
	unsigned int start, end, startloop, endloop;
	uint32_t pos;
	float frac;
	float ratio;
	adsr_t *ampvol, *moddvol;
	int midi;
	int velocity;
	int chid;
	float panLeft, panRight;
	short attenuate;
	lpf_t *lpf;
	struct _voice *next;
} voice;

typedef struct
{
	int program_number;
	unsigned short midi_volume;
	unsigned short midi_pan;
	PresetZones pzset;
	voice *voices;
} channel_t;

typedef struct _ctx
{
	int sampleRate;
	uint16_t currentFrame;
	int samples_per_frame;
	int refcnt;
	float mastVol;
	channel_t channels[16];
	float *outputbuffer;
	FILE *outputFD;
} ctx_t;

void applyZone(zone_t *z, int midi, int vel, int cid);

static ctx_t *g_ctx;
int get_sf(int channelNumer, int key, int vel);

adsr_t *newEnvelope(short centAtt, short centRelease, short centDecay, short sustain, int sampleRate);
float envShift(adsr_t *env);
void adsrRelease(adsr_t *env);

voice *newVoice(zone_t *z, int midi, int vel, int cid);
ctx_t *init_ctx();
void setProgram(int channelNumber, int presetId);
void noteOn(int channelNumber, int midi, int vel, unsigned long when);
void noteOff(int i, int midi);
void render(ctx_t *ctx);

void render_fordr(ctx_t *ctx, float duration, void (*cb)(ctx_t *ctx));