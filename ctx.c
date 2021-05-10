#include <math.h>
#ifndef read2_h
#include "read2.c"
#endif
#include "string.h"
#include "envelope.c"
typedef struct _voice
{
	zone_t *z;
	uint32_t pos;
	float frac;
	float ratio;
	adsr_t *ampvol;
	int midi;
	int velocity;
	int chid;
	struct _voice *next;
} voice;
typedef struct
{
	voice voices[2];
	voice *queue;
	int program_number;
	float midi_gain_cb;
	float midi_pan;
} channel_t;

typedef struct _ctx
{
	int sampleRate;
	int currentFrame_start;
	int samples_per_frame;
	channel_t channels[16];
	float output[128 * 2];
} ctx_t;

ctx_t *init_ctx()
{
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame_start = 0;
	ctx->samples_per_frame = 128 * 2;
	for (int i = 0; i < 16; i++)
	{
		ctx->channels[i].program_number = 0;
		ctx->channels[i].midi_gain_cb = 0;
	}
	return ctx;
}

void noteOn(ctx_t *ctx, int channelNumber, int midi, int vel)
{
	int programNumber = ctx->channels[channelNumber].program_number;

	zone_t *z = index22(programNumber & 0x7f, programNumber & 0x80, midi, vel);
	int i = 0;
	while (i < 2 && z != NULL)
	{

		voice v;
		printf("%d %d %d %d", z->VolEnvAttack, z->VolEnvDecay, z->VolEnvDecay, z->VolEnvSustain);
		v.ampvol = newEnvelope(z->VolEnvAttack, z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain, 48000);
		printf("%d %f", v.ampvol->att_steps, v.ampvol->decay_rate);
		short rt = z->OverrideRootKey > -1 ? z->sample->originalPitch : z->sample->originalPitch;
		float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
		float octaveDivv = ((float)midi * 100 - sampleTone) / 1200.0f;
		v.ratio = 1.0f * pow(2.0f, octaveDivv) * z->sample->sampleRate / 48000;

		v.pos = z->sample->start;
		v.frac = 0.0f;
		v.z = z;
		v.midi = midi;
		v.velocity = vel;
		ctx->channels[channelNumber].voices[i++] = v;
	}
}
void noteOff(ctx_t *ctx, int channelNumber, int midi)
{
	for (int i = 0; i < 4; i++)
	{
		voice v = ctx->channels[channelNumber].voices[i];
		if (v.midi == midi)
			adsrRelease(v.ampvol);
	}
}

void loop(voice *v, channel_t *ch, ctx_t *ctx)
{
	uint32_t loopLength = v->z->sample->endloop - v->z->sample->startloop;
	int cb_attentuate = v->z->Attenuation; // - ch->midi_gain_cb;
	uint16_t pan = ch->midi_pan > 0 ? ch->midi_pan * 960 / 127 : v->z->Pan;
	float panLeft = 1.01;
	float panright = 0.9;
	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float gain = f1 + (f2 - f1) * v->frac;
		float tt = envShift(v->ampvol) + cb_attentuate;
		//	printf("\n atten %d f", tt);

		float mono = gain * centdblut(tt); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		ctx->output[2 * i] += mono * (1 + pan / 1000);
		ctx->output[2 * i + 1] += mono * (1 - pan / 1000);
		v->frac += v->ratio;
		while (v->frac >= 1.0f)
		{
			v->frac--;
			v->pos++;
		}
		if (v->pos >= v->z->sample->endloop)
		{
			v->pos = v->pos - loopLength;
		}
	}
}
void render(ctx_t *ctx, FILE *stdout)
{
	bzero(ctx->output, sizeof(float) * ctx->samples_per_frame);
	for (int ch = 0; ch < 16; ch++)
	{
		for (int i = 0; i < 4; i++)
		{
			voice v = ctx->channels[ch].voices[i];
			loop(v, ctx->channels[ch], ctx);
		}
	}
	fwrite(ctx->output, 128, 4, stdout);
}