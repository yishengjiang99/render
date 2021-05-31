#include <assert.h>
#include "runtime.c"
#include <stdio.h>

int main()
{
	readsf(fopen("file.sf2", "rb"));
	ctx_t *ctx = init_ctx();
	ctx->outputFD = fopen("o.pcm", "w");
	ctx->channels[0].program_number = 60;
	ctx->channels[1].program_number = 0;

	printf("noteon");
	noteOn(1, 66, 44, 0);
	noteOn(0, 58, 44, 0);

	render_fordr(ctx, 1.0, NULL);
	noteOff(1, 66);
	render_fordr(ctx, 1.0, NULL);
	noteOff(0, 58);

	pclose(ctx->outputFD);
	system("ffplay -f f32le -i o.pcm");
}