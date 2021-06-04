#include <assert.h>
#include "sf2.h"

#include "runtime.h"
#include <stdio.h>

int main()
{
	readsf(fopen("file.sf2", "rb"));
	ctx_t *ctx = init_ctx();
	ctx->outputFD = fopen("o.pcm", "w");
	setProgram(0, 0);
	setProgram(1, 2);

	printf("noteon");
	noteOn(1, 66, 44, 0);
	noteOn(0, 58, 44, 0);
	printf("\n%d", g_ctx->channels[0].voices->ampvol->decay_steps);
	printf("\n%d", g_ctx->channels[0].voices->ampvol->release_steps);

	render_fordr(ctx, 1.0, NULL);
	noteOff(1, 66);
	render_fordr(ctx, 1.0, NULL);
	noteOff(0, 58);
	render_fordr(ctx, 4.0, NULL);

	pclose(ctx->outputFD);
	system("ffplay -f f32le -i o.pcm");
}