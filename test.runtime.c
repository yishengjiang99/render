#include "runtime.c"
#include <assert.h>
#include "call_ffp.c"
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
	g_ctx->channels->program_number = phdrs[0].pid;
	noteOn(0, 55, 77, 0);
	assert(g_ctx->channels->voices != NULL);
	assert(g_ctx->channels->voices->next == NULL);

#define printvoice(v)                        \
	printf("\n %u,%u,\t%u %u,\t %u %u",        \
				 v->sample->start, v->start,         \
				 v->sample->startloop, v->startloop, \
				 v->sample->endloop, v->endloop);

	printvoice(g_ctx->channels[0].voices);
	g_ctx->outputFD = ffp(2, 48000);
	g_ctx->channels[0].voices = NULL;
	for (int i = 0; i < 8; i++)
	{

		setProgram(i, i);
		for (int m = 57; m < 78; m++)
		{
			printf("\n***%d %d\n", m, i);
			noteOn(i, m, 55, 0);
			render_fordr(g_ctx, .2, NULL);
			noteOff(i, m);
			render_fordr(g_ctx, 1, NULL);
		}
		break;
	}
}
