#include "runtime.c"
#include <assert.h>
#include "sf2.h"
#include <stdlib.h>
int main()
{
	readsf(fopen("sm.sf2", "rb"));
	ctx_t *ctx = init_ctx();
	ctx->channels[0].program_number = 2;
	noteOn(0, 46, 77, 0);
	assert(ctx->refcnt == 1);
	// assert(ctx->channels[0].voices[0].ampvol->att_steps > 0);
	// adsrRelease(g_ctx->channels[0].voices->ampvol);
	// render_fordr(g_ctx, g_ctx->channels[0].voices->ampvol->release_steps * output_sampleRate + 1200, NULL);
	// assert(g_ctx->refcnt == 0);

	// noteOn(0, 46, 44, 0);
	// noteOn(0, 48, 44, 0);
	// noteOn(0, 60, 44, 0);
	// render_fordr(g_ctx, 1.0, NULL);

	return 0;
}