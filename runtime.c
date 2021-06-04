#include <stdlib.h>
#include <assert.h>
#include "sf2.h"

#include "luts.c"
#include "lpf.c"
#define output_sampleRate 48000
#define dspbuffersize 128
typedef struct
{
	uint32_t att_steps, decay_steps, release_steps, delay_steps, hold_steps;
	unsigned short sustain;
	float db_attenuate;
	float att_rate, decay_rate, release_rate;
} adsr_t;
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
#ifndef lut
#define lut
static inline float p2over1200(float x)
{
	if (x < -12000)
		return 0;
	if (x < 0)
		return 1.f / p2over1200(-x);
	else if (x > 1200.0f)
	{
		return 2 * p2over1200(x - 1200.0f);
	}
	else
	{
		return p2over1200LUT[(unsigned short)(x)];
	}
}
static inline float centdblut(int x)
{
	if (x < 0)
		x = 0;
	if (x > 960)
		x = 960;

	return centdbLUT[x];
}
static inline float midiCBlut(int midi)
{
	if (midi < 0)
		return -960.0f;
	if (midi > 960)
		return 0.0f;
	return midiCB[midi];
}
static inline uint32_t timecent2steps(short tc, uint32_t samplerate)
{
	return p2over1200(tc) * samplerate;
}
static inline float centtone2freq(short ct)
{
	return p2over1200(ct - 6900) * 440.0f;
}
static inline float ct2relativePitch(short ct)
{
	return p2over1200(ct);
}

static inline float panLeftLUT(short Pan)
{
	if (Pan < -500)
	{
		return 1;
	}
	if (Pan > 500)
	{
		return 0.0f;
	}
	else
	{
		return 0.5f + pan[Pan + 500];
	}
}
#endif

short attrs[240];
//(presetZones[i].zones[j].KeyRange & 0x7f00) >> 8)
int get_sf(int channelNumer, int key, int vel)
{

	channel_t ch = g_ctx->channels[channelNumer];
	zone_t *zones = ch.pzset.zones;
	int found = 0;
	for (int i = 0; i < ch.pzset.npresets; i++, zones++)
	{
		if (zones->VelRange.lo > vel || zones->VelRange.hi < vel)
			continue;
		if (zones->KeyRange.lo > key || zones->KeyRange.hi < key)
			continue;
		applyZone(zones, key, vel, channelNumer);
		found++;
	}
	return found;
}

adsr_t *newEnvelope(short centAtt, short centRelease, short centDecay, short sustain, int sampleRate)
{
	adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
	env->att_steps = p2over1200(centAtt) * sampleRate;
	env->decay_steps = p2over1200(centDecay) * sampleRate;
	env->release_steps = p2over1200(centRelease) * sampleRate;
	env->att_rate = -960.0f / env->att_steps;
	env->decay_rate = ((float)1.0f * sustain) / (env->decay_steps);
	env->release_rate = (float)(1000 - sustain) / (env->release_steps);
	env->db_attenuate = 950.0f;

	return env;
}
float envShift(adsr_t *env)
{
	if (env->att_steps > 0)
	{
		env->att_steps--;
		env->db_attenuate += env->att_rate;
	}
	else if (env->decay_steps > 0)
	{
		env->decay_steps--;
		env->db_attenuate += env->decay_rate;
	}
	else if (env->release_steps > 0)
	{
		env->release_steps--;
		env->db_attenuate += env->release_rate;
	}
	//printf("%f", env->db_attenuate);
	if (env->db_attenuate > 960)
	{
		if (env->att_steps <= 0)
		{
			env->decay_steps = 0;
		}
		else
		{
			env->db_attenuate = 960.0f;
		}
	}
	if (env->db_attenuate < 0.0)
	{
		env->db_attenuate = 0.0f;
	}
	return env->db_attenuate;
}
void adsrRelease(adsr_t *env)
{
	env->decay_steps = 0;
	env->att_steps = 0;
	env->release_steps = env->db_attenuate / 1000.0f * (env->release_steps);
}
static inline float hermite4(float frac_pos, float xm1, float x0, float x1, float x2)
{
	const float c = (x1 - xm1) * 0.5f;
	const float v = x0 - x1;
	const float w = c + v;
	const float a = w + v + (x2 - x0) * 0.5f;
	const float b_neg = w + a;

	return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

#define trval (*tr)
#define trshift &((*tr)->next)
void loop(voice *v, float *output)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = v->z->Attenuation;

	for (int i = 0; i < g_ctx->samples_per_frame; i++)
	{
		float fm1 = *(sdta + v->pos - 1);
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float f3 = *(sdta + v->pos + 2);

		float o1 = *(output + 2 * i + 1);
		float o2 = *(output + 2 * i);
		float gain = hermite4(v->frac, fm1, f1, f2, f3);
		//f1 + (f2 - f1) * v->frac;
		if (v->lpf != NULL)
		{
			gain = process_input(v->lpf, gain);
		}

		float mono = gain * centdblut(envShift(v->ampvol)); //+ v->attenuate);
		*(output + 2 * i) += mono * v->panRight;
		*(output + 2 * i + 1) += mono * v->panLeft;

		v->frac += v->ratio;
		while (v->frac >= 1.0f)
		{
			v->frac--;
			v->pos++;
		}
		while (v->pos >= v->endloop)
		{
			if (v->z->SampleModes == 0)
			{
				v->ampvol->release_steps = 0;
				return;
			}
			v->pos = v->pos - loopLength + 1;
		}
	}
}
void insertV(voice **start, voice *nv)
{
	voice **tr = start;
	while (*tr)
		tr = &((*tr)->next);
	*tr = nv;
}
float calcratio(zone_t *z, shdrcast *sh, int midi)
{
	short rt = z->OverrideRootKey > -1 ? z->OverrideRootKey : sh->originalPitch;
	float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
	float octaveDivv = (float)midi * 100 - sampleTone;
	return p2over1200(octaveDivv) * (float)sh->sampleRate / g_ctx->sampleRate;
}
voice *newVoice(zone_t *z, int midi, int vel)
{
	voice *v = (voice *)malloc(sizeof(voice));
	v->next = NULL;
	shdrcast *sh = (shdrcast *)(shdrs + z->SampleId);
	v->z = z;
	v->sample = sh;
	v->start = sh->start + ((unsigned short)(z->StartAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->StartAddrOfs & 0x7f);
	v->end = sh->end + ((unsigned short)(z->EndAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndAddrOfs & 0x7f);
	v->endloop = sh->endloop + ((unsigned short)(z->EndLoopAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndLoopAddrOfs & 0x7f);
	v->startloop = sh->startloop + (unsigned short)(z->StartLoopAddrCoarseOfs & 0x7f << 15) + (unsigned short)(z->StartLoopAddrOfs & 0x7f);
	v->ampvol = newEnvelope(z->VolEnvAttack, z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain, 48000);
	v->ratio = calcratio(z, sh, midi);
	v->pos = v->start;
	v->frac = 0.0f;
	v->z = z;
	v->midi = midi;
	v->velocity = vel;
	v->panLeft = panLeftLUT(z->Pan);
	v->panRight = 1 - v->panLeft;
	v->attenuate = z->Attenuation + velCB[vel];

	if (z->FilterFc < 13000)
	{
		v->lpf = (lpf_t *)malloc(sizeof(lpf_t));
		newLpf(v->lpf, centtone2freq(z->FilterFc), g_ctx->sampleRate);
	}
	g_ctx->refcnt++;
	//insertV(&(g_ctx->channels[channelNumber].voices), v);
}

ctx_t *init_ctx()
{
	g_ctx = (ctx_t *)malloc(sizeof(ctx_t));
	g_ctx->sampleRate = output_sampleRate;
	g_ctx->currentFrame = 0;
	g_ctx->samples_per_frame = dspbuffersize;
	for (int i = 0; i < 16; i++)
	{
		g_ctx->channels[i].program_number = 0;
		g_ctx->channels[i].midi_volume = 1.0f;
		g_ctx->channels[i].voices = NULL;
	}
	g_ctx->refcnt = 0;
	g_ctx->mastVol = 1.0f;
	g_ctx->outputbuffer = (float *)malloc(sizeof(float) * g_ctx->samples_per_frame * 2);
	return g_ctx;
}
void setProgram(int channelNumber, int presetId)
{
	g_ctx->channels[channelNumber].pzset = findByPid(presetId, channelNumber == 9 ? 128 : 0);
	g_ctx->channels[channelNumber].program_number = presetId;
}
void noteOn(int channelNumber, int midi, int vel, unsigned long when)
{
	//	assert(g_ctx->channels[channelNumber].pzset.npresets > 0);
	get_sf(channelNumber, midi, vel);
}

void noteOff(int i, int midi)
{
	channel_t ch = g_ctx->channels[i];

	voice *v = ch.voices;
	if (v != NULL)
	{

		if (v->midi == midi)
		{
			adsrRelease(v->ampvol);
		}
		v = v->next;
	}
}
void rgo()
{
	render(g_ctx);
}
void render(ctx_t *ctx)
{
	bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);
	int vs = 0;
	for (int i = 0; i < 16; i++)
	{
		channel_t ch = g_ctx->channels[i];
		voice *v = ch.voices;
		voice *prev = ch.voices;
		if (v != NULL)
		{
			loop(v, g_ctx->outputbuffer);

			if (v->ampvol->db_attenuate > 960 || v->ampvol->release_steps <= 0)
			{
				prev->next = v->next;
			}
			v = v->next;
		}
	}

	ctx->currentFrame++;
	float maxv = 1.0f, redradio = 1.0f;
	float postGain = ctx->mastVol;
	for (int i = 0; i < ctx->samples_per_frame; i++)
	{
		ctx->outputbuffer[i] *= postGain;
		float absf = ctx->outputbuffer[i] > 0.f ? ctx->outputbuffer[i] : -1 * ctx->outputbuffer[i];

		if (absf >= maxv)
		{
			//fprintf(stderr, "clipping at %f\n", absf);
			maxv = absf;
		}
	}

	if (ctx->outputFD != NULL)
		fwrite(ctx->outputbuffer, ctx->samples_per_frame * 2, 4, ctx->outputFD);
}
void render_fordr(ctx_t *ctx, float duration, void (*cb)(ctx_t *ctx))
{
	while (duration > 0.0f)
	{
		render(ctx);
		if (cb)
			cb(ctx);
		duration -= 0.00266f;
	}
}