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
void loop(voice *v, float *output, channel_t ch, int n);
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
	ctx->voices = 0;
	ctx->fadeouts = 0;
	ctx->refcnt = 0;
	ctx->mastVol = 1.0f;
	ctx->outputFD = NULL;
	ctx->outputbuffer = (float *)malloc(sizeof(float) * ctx->samples_per_frame * 2);
	return ctx;
}

void noteOn(ctx_t *ctx, int channelNumber, int midi, int vel, unsigned long when)
{

	int programNumber = ctx->channels[channelNumber].program_number;
	ctx->refcnt++;
	get_sf(programNumber & 0x7f, programNumber & 0x80, midi, vel, &(ctx->voices), channelNumber);
}

void noteOff(ctx_t *ctx, int ch, int midi)
{
	node *voices = ctx->voices;
	node **tracer = &voices;
	node *v = remove_node(&voices, ch);
	ctx->refcnt--;
	if (v && v->voice != NULL)
	{
		adsrRelease(v->voice->ampvol);
		insert_node(&(ctx->fadeouts), newNode(v->voice, 4));
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
		loop(v, ctx->outputbuffer, ctx->channels[v->chid], ctx->refcnt);
		tracer = &(*tracer)->next;
	}
	// voices = ctx->fadeouts;
	// tracer = &voices;
	// while (*tracer && (*tracer)->voice)
	// {
	// 	voice *v = (*tracer)->voice;
	// 	loop(v, ctx->outputbuffer, ctx->channels[v->chid], ctx->refcnt);
	// 	if (v->ampvol->release_rate < 128)
	// 	{
	// 		ctx->fadeouts = (*tracer)->next;
	// 		break;
	// 	}
	// 	tracer = &(*tracer)->next;
	// }
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
		// for (int i = 0; i < ctx->samples_per_frame; i++)
		// {
		// 	ctx->outputbuffer[i] = ctx->outputbuffer[i] / maxv;
		// 	if (ctx->outputbuffer[i] > 1.0f)
		// 		ctx->outputbuffer[i] = 1.0f;
		// }
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

void loop(voice *v, float *output, channel_t ch, int n)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = v->z->Attenuation + velCB[v->velocity] + ch.midi_gain_cb;
	//printf("\n%f", centdblut(attentuate));
	short pan = v->z->Pan;

	float panLeft = sqrtf(0.5f - pan / 1000.0f);
	float panright = sqrtf(0.5f + pan / 1000.0f);
	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float gain = f1 + (f2 - f1) * v->frac;

		float mono = gain * centdblut(envShift(v->ampvol) + (int)attentuate); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		*(output + 2 * i) += mono * panright;
		*(output + 2 * i + 1) += mono * panLeft;
		if (mono > 1.0f)
		{
			printf("\nmidichan %d %f %f\nn=%f ", n, centdblut(v->z->Attenuation + velCB[v->velocity] + ch.midi_gain_cb), ch.midi_gain_cb, velCB[v->velocity]);
		}
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
