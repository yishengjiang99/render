#include "sf2.h"


#ifndef lut
#define lut

#include "luts.c"
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


#ifndef adsrr
# define adsrr

#define adsr 1
#define fmax(a, b) a > b ? a : b
typedef struct
{
	uint32_t att_steps, decay_steps, release_steps, delay_steps, hold_steps;
	unsigned short sustain;
	float db_attenuate;
	float att_rate, decay_rate, release_rate;
} adsr_t;

adsr_t *newEnvelope(short centAtt, short centRelease, short centDecay, short sustain, int sampleRate)
{
	adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
	env->att_steps = fmax(p2over1200(centAtt) * sampleRate, 12);
	env->decay_steps = fmax(p2over1200(centDecay) * sampleRate, 2);
	env->release_steps = fmax(p2over1200(centRelease) * sampleRate, 2);
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
	}
	else if (env->release_steps > 0)
	{
		env->release_steps--;
		env->db_attenuate += env->release_rate;
	}
	if (env->db_attenuate > 960)
	{
		env->db_attenuate = 960.0f;
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


#endif

#ifndef VOICE_C
#define VOICE_C
#include "ctx.h"

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
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MID(x, y, z) MAX((x), MIN((y), (z)))

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

		float mono = gain * centdblut(envShift(v->ampvol)); // + attentuate); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
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

void applyZone(voice *v, zone_t *z, int midi, int vel)
{
	shdrcast *sh = (shdrcast *)(shdrs + z->SampleId);
	v->z = z;
	v->sample = sh;
	v->start = sh->start + ((unsigned short)(z->StartAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->StartAddrOfs & 0x7f);
	v->end = sh->end + ((unsigned short)(z->EndAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndAddrOfs & 0x7f);
	v->endloop = sh->endloop + ((unsigned short)(z->EndLoopAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndLoopAddrOfs & 0x7f);
	v->startloop = sh->startloop + (unsigned short)(z->StartLoopAddrCoarseOfs & 0x7f << 15) + (unsigned short)(z->StartLoopAddrOfs & 0x7f);
	v->ampvol = newEnvelope(z->VolEnvAttack, z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain, 48000);

	short rt = z->OverrideRootKey > -1 ? z->OverrideRootKey : sh->originalPitch;
	float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
	float octaveDivv = ((float)midi * 100 - sampleTone) / 1200.0f;
	v->ratio =

			p2over1200(octaveDivv) * sh->sampleRate / 48000;

	v->pos = v->start;
	v->frac = 0.0f;
	v->z = z;
	v->midi = midi;
	v->velocity = vel;
	v->panLeft = panLeftLUT(z->Pan);
	v->panRight = 1 - v->panLeft;
}
voice *newVoice(zone_t *z, int midi, int vel)
{
	voice *v = (voice *)malloc(sizeof(voice));
	applyZone(v, z, midi, vel);
	return v;
}

#endif