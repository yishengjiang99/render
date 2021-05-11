#ifndef read2_h
#include "sf2.c"
#endif
#include "string.h"
#ifndef adsr
#include "envelope.c"
#endif
#ifndef voice_h
#include "voice.c"

#endif
typedef struct
{
	node *fadeouts; //voices after release
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
	node *voices;
	float output[128 * 2];
	FILE *outputFD;
} ctx_t;

ctx_t *init_ctx(FILE *fdout)
{
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame_start = 0;
	ctx->samples_per_frame = 128 * 2;
	for (int i = 0; i < 16; i++)
	{
		ctx->channels[i].program_number = 0;
		ctx->channels[i].midi_gain_cb = 0;
		ctx->channels[i].fadeouts = 0;
	}
	ctx->voices = 0;
	ctx->outputFD = fdout;
	return ctx;
}

void noteOn(ctx_t *ctx, int channelNumber, int midi, int vel)
{
	int programNumber = ctx->channels[channelNumber].program_number;

	get_sf(programNumber & 0x7f, programNumber & 0x80, midi, vel, &(ctx->voices));
}

void noteOff(ctx_t *ctx, int ch, int midi)
{
	node *voices = ctx->voices;
	node **tracer = &voices;
	node *fadeout = ctx->channels[ch].fadeouts;
	while (*tracer && (*tracer)->voice != NULL)
	{
		voice *v = (*tracer)->voice;
		if (v->midi == midi)
		{
			adsrRelease(v->ampvol);
			(*tracer) = (*tracer)->next;
			insert_node(&fadeout, newNode(v, 1));
		}
	}
}

void loop(voice *v, channel_t *ch, ctx_t *ctx)
{
	uint32_t loopLength = v->endloop - v->startloop;
	int cb_attentuate = v->z->Attenuation; // - ch->midi_gain_cb;
	short pan = v->z->Pan;
	float panLeft = sqrtf(0.5f - pan / 1000.0f);
	float panright = sqrtf(0.5f + pan / 1000.0f);
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
		if (v->pos >= v->sample->endloop)
		{
			v->pos = v->pos - loopLength;
		}
	}
}
void render(ctx_t *ctx)
{
	bzero(ctx->output, sizeof(float) * ctx->samples_per_frame);

	node *voices = ctx->voices;
	node **tracer = &voices;
	while (*tracer && (*tracer)->voice)
	{
		voice *v = (*tracer)->voice;
		loop(v, &ctx->channels[v->chid], ctx);
		tracer = &(*tracer)->next;
	}
	fwrite(ctx->output, 128 * 2, 4, ctx->outputFD);
}