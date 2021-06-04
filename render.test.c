#include <assert.h>
#include "sf2.c"
#include "runtime.c"
#include <stdio.h>

int main()
{
	short attrs[60] = defattrs;
	zone_t *z = (zone_t *)attrs;
	printf("%d,%d", z->VelRange.lo, z->VelRange.hi);

	init_ctx();
	readsf(fopen("GeneralUserGS.sf2", "rb"));
	ctx_t *ctx = init_ctx();
	ctx->outputFD = fopen("o.pcm", "w");
	ctx->channels[0].program_number = 60;
	ctx->channels[1].program_number = 0;
	setProgram(0, 0);
	noteOn(0, 77, 99, 0);

	render_fordr(g_ctx, 1.0, NULL);

	return 1;

	//assert(calcratio(&z, &sh, 55) == powf(2.0f, (71.0f - 55) / 1200.0f));
}