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
#include "ctx.h"
#include "shift.c"

ctx_t *init_ctx()
{
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame = 0;
	ctx->samples_per_frame = 128;
	for (int i = 0; i < 16; i++)
	{
		ctx->channels[i].program_number = i * 7;
		ctx->channels[i].midi_gain_cb = 90;
	}
	ctx->voices = (voice *)malloc(sizeof(voice) * 16);
	ctx->refcnt = 0;
	ctx->mastVol = 1.0f;
	ctx->outputFD = NULL;
	ctx->outputbuffer = (float *)malloc(sizeof(float) * ctx->samples_per_frame * 2);
	return ctx;
}
void loopctx(voice *v, ctx_t *ctx)
{
	loop(v, ctx->outputbuffer);
}
void noteOn(ctx_t *ctx, int channelNumber, int midi, int vel, unsigned long when)
{
	int programNumber = ctx->channels[channelNumber].program_number;
	voice *v1 = ctx->voices + 2 * channelNumber + 0;
	voice *v2 = ctx->voices + 2 * channelNumber + 1;
	zone_t *z = get_sf(programNumber & 0x7f, programNumber & 0x80, midi, vel);
	if (z)
		applyZone(ctx->voices + 2 * channelNumber + 0, z, midi, vel);
}

void noteOff(ctx_t *ctx, int ch, int midi)
{
	if ((ctx->voices + ch * 2)->midi == midi)
	{
		adsrRelease((ctx->voices + ch * 2)->ampvol);
	}
}
void render(ctx_t *ctx)
{
	bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);
	for (int i = 0; i < 16; i++)
	{
		voice *v = ctx->voices + i;

		if ((ctx->voices + i)->z != NULL)
			loopctx(ctx->voices + i, ctx);
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
			fprintf(stderr, "clipping at %f\n", absf);
			maxv = absf;
		}
	}
	if (maxv > 1.0f)
	{
		for (int i = 0; i < ctx->samples_per_frame; i++)
		{
			ctx->outputbuffer[i] = ctx->outputbuffer[i] / maxv;
			if (ctx->outputbuffer[i] > 1.0f)
				ctx->outputbuffer[i] = 1.0f;
		}
	}
	if (ctx->outputFD)
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
