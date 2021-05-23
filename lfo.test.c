#include <assert.h>
#include "lfo.c"
#include <stdlib.h>
#include <math.h>
#include "sf2.c"
#include "call_ffp.c"
#include "voice.c"
#include "shift.c"
#include "ctx.c"
#include <stdio.h>
#define passert(e)          \
	printf("%f", (float)(e)); \
	assert(e);
int main()
{
	readsf(fopen("FluidR3_GM.sf2", "rb"));
	ctx_t *ctx = init_ctx();

	zone_t *z = get_sf(phdrs[0].pid, phdrs[0].bankId, 55, 55);
	voice *v = (voice *)malloc(sizeof(voice));
	applyZone(v, z, 55, 55);
	//FILE *o = wavepic("pic.png");
	float *buf = (float *)malloc(sizeof(float) * dspbuffersize * 2);

	v->ratio = 1.0f;
	loop(v, buf);

	printf("%u %u", v->endloop - v->startloop, 1); //v->panLeft, 1;
	return 1;
}