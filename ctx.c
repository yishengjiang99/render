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
#include "shift.c"

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

ctx_t *init_ctx()
{
	/*
	(-200.0 / 960) *
                    Math.log10((note.velocity * note.velocity) / (127 * 127))
										*/
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame_start = 0;
	ctx->samples_per_frame = 128;
	for (int i = 0; i < 16; i++)
	{
		ctx->channels[i].program_number = 0;
		ctx->channels[i].midi_gain_cb = 0;
	}
	ctx->voices = 0;
	ctx->fadeouts = 0;
	ctx->outputFD = NULL;
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
	insert_node(&(ctx->fadeouts), newNode(v->voice, 4));
}
void render(ctx_t *ctx)
{
	bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);

	node *voices = ctx->voices;
	node **tracer = &voices;
	while (*tracer && (*tracer)->voice)
	{
		voice *v = (*tracer)->voice;
		loop(v, ctx->outputbuffer);
		tracer = &(*tracer)->next;
	}
	voices = ctx->fadeouts;
	tracer = &voices;
	while (*tracer && (*tracer)->voice)
	{
		voice *v = (*tracer)->voice;
		loop(v, ctx->outputbuffer);
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