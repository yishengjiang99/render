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
	zone_t z;
	shdrcast sh;
	z.OverrideRootKey = 70;
	z.FineTune = 100;
	sh.sampleRate = 24000.0f;
	printf("\n %f %f %d\n", calcratio(&z, &sh, 55), powf(2.0f, (55.0f - 71.0f) / 1200.0f), z.OverrideRootKey);

	printf("\n  %f %f %d", calcratio(&z, &sh, 56), powf(2.0f, (56.0f - 71.0f) / 1200.0f), z.OverrideRootKey);

	printf("\n  %f %f %d", calcratio(&z, &sh, 33), powf(2.0f, (56.0f - 71.0f) / 1200.0f), z.OverrideRootKey);
	//assert(calcratio(&z, &sh, 55) == powf(2.0f, (71.0f - 55) / 1200.0f));
}