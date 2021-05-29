#include "envelope.c"
#include "voice.c"
#include "sf2.c"
#include "string.h"

#include "ctx.h"
#include "shift.c"

ctx_t *init_ctx()
{
	ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
	ctx->sampleRate = 48000;
	ctx->currentFrame = 0;
	ctx->samples_per_frame = 128;
	for (int i = 0; i < 32; i++)
	{
		ctx->channels[i].program_number = 0;
		ctx->channels[i].midi_volume = 1.0f;
	}
	ctx->voices = (voice *)malloc(sizeof(voice) * 16 * 2);
	ctx->refcnt = 0;
	ctx->mastVol = 1.0f;
	ctx->outputFD = NULL;
	ctx->outputbuffer = (float *)malloc(sizeof(float) * ctx->samples_per_frame * 2);
	return ctx;
}
void loopctx(voice *v, ctx_t *ctx, int flag)
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
	{
		applyZone(v1, z, midi, vel);
		v1->attenuate += ctx->channels[channelNumber].midi_volume + velCB[vel];
	}
	if (z + 1)
	{
		applyZone(v2, z, midi, vel);
		v2->attenuate += ctx->channels[channelNumber].midi_volume + velCB[vel];
	}
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
	int vs = 0;
	for (int i = 0; i < 32; i++)
	{
		voice *v = ctx->voices + i;
		if (v->z && v->ampvol->release_steps > 0)
		{
			loopctx(v, ctx, 1);
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
