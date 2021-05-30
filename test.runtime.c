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
	g_ctx->channels->program_number = phdrs[0].pid;
	noteOn(0, 55, 77, 0);
	assert(g_ctx->voices != NULL);
	assert(g_ctx->voices->z != NULL);
	assert(g_ctx->refcnt = 2);
	assert(g_ctx->voices->ampvol->att_steps > 0);
	//	printf("\n%d", g_ctx->voices->sample->start);

#define printvoice(v)                        \
	printf("\n %u,%u,\t%u %u,\t %u %u",        \
				 v->sample->start, v->start,         \
				 v->sample->startloop, v->startloop, \
				 v->sample->endloop, v->endloop);
	noteOn(0, 55, 77, 0);

	printvoice(g_ctx->voices);
	for (int i = 0; i < nphdrs - 1; i++)
	{
		if (phdrs[i].bankId != 0)
			continue;
		g_ctx->channels[i].program_number = phdrs[i].pid;
		for (int m = 32; m < 78; m++)
		{
			//	printf("\n***%d %d\n", m, i);
			noteOn(i, m, 55, 0);
			noteOff(i, m);
		}

		g_ctx->voices = NULL;
	}
}
