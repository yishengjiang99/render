#include <stdlib.h>
#include <assert.h>
#include "sf2.c"
#include "call_ffp.c"
#include "minisdl_audio.h"
#include "luts.c"
#define output_sampleRate 48000
#define dspbuffersize 32
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
#define output_sampleRate 48000
#define dspbuffersize 32

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
void sanitizedInsert(short *attrs, int i, pgen_t *g)
{
	switch (i % 60)
	{
	case StartAddrOfs:
	case EndAddrOfs:
	case StartLoopAddrOfs:
	case EndLoopAddrOfs:
	case StartAddrCoarseOfs:
	case EndAddrCoarseOfs:
	case StartLoopAddrCoarseOfs:
	case EndLoopAddrCoarseOfs:
	case OverrideRootKey:
		attrs[i] = g->val.uAmount & 0x7f;
		break;
	default:
		attrs[i] = g->val.shAmount;
		break;
	}
}
void get_sf(int channelNumer, int key, int vel)
{
	short attrs[240] = {0};
	int bkid = channelNumer == 9 ? 128 : 0;
	int pid = g_ctx->channels[channelNumer].program_number;
	int found = 0;

	int instID = -1, lastSampId = -1;
	enum
	{
		default_pbg_cache_index = 0,
		pbg_attr_cache_index = 60,
		default_ibagcache_idex = 120,
		ibg_attr_cache_index = 180

	};
	for (int i = 0; i < nphdrs - 1; i++)
	{
		if (phdrs[i].bankId != bkid || phdrs[i].pid != pid)
			continue;
		int lastbag = phdrs[i + 1].pbagNdx;
		bzero(&attrs[default_pbg_cache_index], 240 * sizeof(short));

		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			int attr_inex = j == phdrs[i].pbagNdx ? default_pbg_cache_index : pbg_attr_cache_index;
			bzero(&attrs[pbg_attr_cache_index], 180 * sizeof(short));

			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;

			for (int k = pgenId; k < lastPgenId; k++)
			{
				pgen *g = pgens + k;
				if (vel > -1 && g->genid == VelRange && (g->val.ranges.lo > vel || g->val.ranges.hi < vel))
					break;
				else if (key > -1 && g->genid == KeyRange && (g->val.ranges.lo > key || g->val.ranges.hi < key))
					break;
				if (g->genid != Instrument)
				{

					sanitizedInsert(attrs, g->genid + attr_inex, g);
				}
				else
				{
					instID = g->val.shAmount;
					sanitizedInsert(attrs, g->genid + attr_inex, g);
					bzero(&attrs[default_ibagcache_idex], 120 * sizeof(short));

					int lastSampId = -1;
					inst *ihead = insts + instID;
					int ibgId = ihead->ibagNdx;
					int lastibg = (ihead + 1)->ibagNdx;
					for (int ibg = ibgId; ibg < lastibg; ibg++)
					{
						bzero((&attrs[0] + ibg_attr_cache_index), 60 * sizeof(short));
						attr_inex = ibg == ibgId ? default_ibagcache_idex : ibg_attr_cache_index;

						lastSampId = -1;
						ibag *ibgg = ibags + ibg;
						pgen_t *lastig = ibg < nibags - 1 ? igens + (ibgg + 1)->igen_id : igens + nigens - 1;
						for (pgen_t *g = igens + ibgg->igen_id; g->genid != 60 && g != lastig; g++)
						{

							if (vel > -1 && g->genid == VelRange && (g->val.ranges.lo > vel || g->val.ranges.hi < vel))
								break;
							if (key > -1 && g->genid == KeyRange && (g->val.ranges.lo > key || g->val.ranges.hi < key))
								break;
							sanitizedInsert(attrs, attr_inex + g->genid, g);

							if (g->genid == SampleId)
							{
								short zoneattr[60] = {0};
								lastSampId = g->val.shAmount; // | (ig->val.ranges.hi << 8);
								for (int i = 0; i < 60; i++)
								{
									if (attrs[ibg_attr_cache_index + i])
									{
										zoneattr[i] = attrs[ibg_attr_cache_index + i];
									}
									else if (attrs[default_ibagcache_idex + i])
									{
										zoneattr[i] = attrs[default_ibagcache_idex + i];
									}
									if (attrs[pbg_attr_cache_index + i])
									{
										zoneattr[i] += attrs[pbg_attr_cache_index + i];
									}
									else if (attrs[default_pbg_cache_index + i])
									{
										zoneattr[i] += attrs[pbg_attr_cache_index + i];
									}
								}
								applyZone((zone_t *)zoneattr, key, vel, channelNumer);
								found++;
							}
						}
					}
				}
			}
		}
	}
	//	printf("#zones %d\n", found);
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
	env->db_attenuate = 960.0f;
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
		//	printf("\n decay %f %d %f", env->db_attenuate, env->decay_steps, env->decay_rate);
	}
	else if (env->release_steps > 0)
	{
		env->release_steps--;
		env->db_attenuate += env->release_rate;
		//	printf("\nrelease %f %d %f", env->db_attenuate, env->release_steps, env->release_rate);
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
}
#define trval (*tr)
#define trshift &((*tr)->next)
void loop(voice *v, float *output)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = v->z->Attenuation;

	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float o1 = *(output + 2 * i + 1);
		float o2 = *(output + 2 * i);
		float gain = f1 + (f2 - f1) * v->frac;

		float mono = gain * centdblut(envShift(v->ampvol));
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
	short rt = z->OverrideRootKey > 0 ? z->OverrideRootKey : sh->originalPitch;
	float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
	float octaveDivv = (float)midi * 100 - sampleTone;
	return p2over1200(octaveDivv) * (float)sh->sampleRate / 48000.0f;
}
void applyZone(zone_t *z, int midi, int vel, int channelNumber)
{

	voice *v = (voice *)malloc(sizeof(voice));
	v->next = g_ctx->channels[channelNumber].voices;
	g_ctx->channels[channelNumber].voices = v;

	shdrcast *sh = (shdrcast *)(shdrs + z->SampleId);
	v->z = z;
	v->sample = sh;
	v->start = sh->start + ((unsigned short)(z->StartAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->StartAddrOfs & 0x7f);
	v->end = sh->end + ((unsigned short)(z->EndAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndAddrOfs & 0x7f);
	v->endloop = sh->endloop + ((unsigned short)(z->EndLoopAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndLoopAddrOfs & 0x7f);
	v->startloop = sh->startloop + (unsigned short)(z->StartLoopAddrCoarseOfs & 0x7f << 15) + (unsigned short)(z->StartLoopAddrOfs & 0x7f);
	v->ampvol = newEnvelope(z->VolEnvAttack, z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain, 48000);
	v->chid = channelNumber;
	v->ratio = calcratio(z, sh, midi);
	v->pos = v->start;
	v->frac = 0.0f;
	v->z = z;
	v->midi = midi;
	v->velocity = vel;
	v->panLeft = panLeftLUT(z->Pan);
	v->panRight = 1 - v->panLeft;
	//insertV(&(g_ctx->channels[channelNumber].voices), v);
}

ctx_t *init_ctx()
{
	g_ctx = (ctx_t *)malloc(sizeof(ctx_t));
	g_ctx->sampleRate = 48000;
	g_ctx->currentFrame = 0;
	g_ctx->samples_per_frame = 128;
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
void loopctx(voice *v, ctx_t *ctx, int flag)
{
	loop(v, ctx->outputbuffer);
}
void noteOn(int channelNumber, int midi, int vel, unsigned long when)
{
	get_sf(channelNumber, midi, vel);
}

void noteOff(int ch, int midi)
{
	voice **tr = &(g_ctx->channels[ch].voices);
	while (*tr)
	{
		if ((*tr)->midi == midi)
		{
			adsrRelease((*tr)->ampvol);
		}
		tr = &(*tr)->next;
	}
}
void render(ctx_t *ctx)
{
	bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);
	int vs = 0;
	for (int i = 0; i < 16; i++)
	{
		voice **tr = &g_ctx->channels[i].voices;
		if (trval)
		{
			if (trval->ampvol->release_steps <= 128)
			{
				trval = trval->next;
			}
			else
			{
				loop(trval, g_ctx->outputbuffer);
			}
			tr = trshift;
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
		duration -= 0.003f;
	}
}

// static void AudioCallback(void *data, Uint8 *stream, int len)
// {

// 	g_ctx->outputbuffer = (float *)stream;
// 	render(g_ctx);
// }
// int main()
// {
// 	SDL_AudioSpec OutputAudioSpec;
// 	OutputAudioSpec.freq = 44100;
// 	OutputAudioSpec.format = AUDIO_F32LSB;
// 	OutputAudioSpec.channels = 2;
// 	OutputAudioSpec.samples = 128;
// 	OutputAudioSpec.callback = AudioCallback;
// 	if (SDL_AudioInit(NULL) < 0)
// 	{
// 		fprintf(stderr, "Could not initialize audio hardware or driver\n");
// 		return 1;
// 	}

// 	if (SDL_OpenAudio(&OutputAudioSpec, &OutputAudioSpec) < 0)
// 	{
// 		fprintf(stderr, "Could not open the audio hardware or the desired audio output format\n");
// 		return 1;
// 	}
// 	init_ctx();
// 	FILE *f = fopen("file.sf2", "rb");
// 	if (!f)
// 		perror("oaffdf");

// 	readsf(f);
// 	g_ctx->channels[0].program_number = phdrs[0].pid;
// 	SDL_PauseAudio(0);

// 	noteOn(0, 55, 33, 0);
// 	float ob[128 * 2];
// 	// Let the audio callback play some sound for 3 seconds
// 	SDL_Delay(3000);
// }