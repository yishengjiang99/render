
const float sampleRate = 44100;
float coeff;
float currentLevel;
#include <math.h>
#include <stdio.h>
void init(float levelBegin, float levelEnd, float releaseTime)
{
	currentLevel = levelBegin;
	coeff = (log(levelEnd) - log(levelBegin)) /
					(releaseTime * sampleRate);
}

void calculateEnvelope(int samplePoints)
{
	for (int i = 0; i < samplePoints; i++)
	{
		currentLevel += coeff * currentLevel;
	}
}
#include <stdint.h>
#include "call_ffp.c"
#include "sf2.c"
#include "runtime.c"
int main()
{
	readsf(fopen("GeneralUserGS.sf2", "rb"));
	short attrs[60] = defattrs;
	zone_t *z = (zone_t *)attrs;
	init_ctx();
	setProgram(9, 0);

	noteOn(9, 44, 12, 0);
	printf("\n%d,", g_ctx->refcnt);

	// g_ctx->outputFD = ffp(2, 48000);
	// shdrcast *s = g_ctx->channels[9].voices[0].sample;

	// g_ctx->channels[9].midi_volume = 128;
	// render_fordr(g_ctx, 10, NULL);
}