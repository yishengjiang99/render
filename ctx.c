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
	node *fadeouts;
	float *outputbuffer;
	FILE *outputFD;
} ctx_t;

ctx_t *init_ctx(FILE *fdout)
{
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame_start = 0;
	ctx->samples_per_frame = 128;
	for (int i = 0; i < 16; i++)
	{
		ctx->channels[i].program_number = 0;
		ctx->channels[i].midi_gain_cb = 0;
		ctx->channels[i].fadeouts = 0;
	}
	ctx->voices = 0;
	ctx->fadeouts = 0;
	ctx->outputFD = fdout;
	ctx->outputbuffer = (float *)malloc(sizeof(float) * ctx->samples_per_frame * 2);
	return ctx;
}

void noteOn(ctx_t *ctx, int channelNumber, int midi, int vel)
{
	int programNumber = ctx->channels[channelNumber].program_number;

	get_sf(programNumber & 0x7f, programNumber & 0x80, midi, vel, &(ctx->voices), channelNumber << 8 | midi);
}

void noteOff(ctx_t *ctx, int ch, int midi)
{
	node *voices = ctx->voices;
	node **tracer = &voices;
	node *v = remove_node(&voices, ch << 8 | midi);
	if (!v)
		printf("not found %d %d", ch, midi);

	adsrRelease(v->voice->ampvol);
	insert_node(&(ctx->fadeouts), v);
}

void loop(voice *v, channel_t *ch, ctx_t *ctx)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = centdblut(v->z->Attenuation);
	short pan = v->z->Pan;

	float panLeft = sqrtf(0.5f - pan / 1000.0f);	//* attentuate;
	float panright = sqrtf(0.5f + pan / 1000.0f); // * attentuate;
	printf("%f\n", attentuate);
	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float gain = f1 + (f2 - f1) * v->frac;

		float mono = gain * centdblut(envShift(v->ampvol)); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		*(ctx->outputbuffer + 2 * i) += mono * panright;
		*(ctx->outputbuffer + 2 * i + 1) += mono * panLeft;
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
void render(ctx_t *ctx)
{
	bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);

	node *voices = ctx->voices;
	node **tracer = &voices;
	while (*tracer && (*tracer)->voice)
	{
		voice *v = (*tracer)->voice;
		loop(v, &ctx->channels[v->chid], ctx);
		tracer = &(*tracer)->next;
	}
	fwrite(ctx->outputbuffer, ctx->samples_per_frame * 2, 4, ctx->outputFD);
}
void render_fordr(ctx_t *ctx, float duration)
{
	while (duration > 0.0f)
	{
		render(ctx);
		duration -= 0.003;
	}
}
void fffclose(ctx_t *x)
{
	fclose(x->outputFD);
}