#include <assert.h>
#include "runtime.c"
#include <stdio.h>

int main()
{
	readsf(fopen("file.sf2", "rb"));
	ctx_t *ctx = init_ctx();
	ctx->outputFD = stdout;
	ctx->channels[0].program_number = 60;
	noteOn(0, 55, 44, 0);
	render_fordr(ctx, 1.0, NULL);
	noteOff(caddr_t)
			get_sf(ctx, 0);
	float *buf = (float *)malloc(sizeof(float) * dspbuffersize * 2);

	v->ratio = 1.0f;
	loop(v, buf);

	printf("%u %u", v->endloop - v->startloop, 1); //v->panLeft, 1;
	return 1;
}