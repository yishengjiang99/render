#include "runtime.c"
#include <assert.h>
void cb(ctx_t *ctx)
{
	for (int i = 0; i < 128; i++)
		printf("\n%f", ctx->outputbuffer[0]);
}
int main()
{
	init_ctx();
	FILE *f = fopen("file.sf2", "rb");
	if (!f)
		perror("oaffdf");

	readsf(f);

	assert(g_ctx != NULL);
	assert(&(g_ctx->channels[0]) != NULL);
	assert(g_ctx->mastVol > 0.0f);
	g_ctx->channels->program_number = 0;
	noteOn(0, 55, 77, 0);
	assert(g_ctx->voices != NULL);
	assert(g_ctx->voices->z != NULL);
	assert(g_ctx->refcnt = 2);
	assert(g_ctx->voices->ampvol->att_steps > 0);
	printf("\n%f", g_ctx->voices->ampvol->att_rate);
	printf("%d %d %f", g_ctx->voices->sample->originalPitch, g_ctx->voices->sample->sampleRate, g_ctx->voices->ratio);
	g_ctx->outputFD = ffp(2, 48000);
	render_fordr(g_ctx, 1.0, &cb);
	render(g_ctx);

	fclose(f);
	fflush(g_ctx->outputFD);
}
