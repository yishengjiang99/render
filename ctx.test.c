#include "ctx.c"
#include <assert.h>

int main()
{
	initLUTs();
	readsf(fopen("file.sf2", "rb"));
	ctx_t *c = init_ctx(stdout);
	assert(c != 0);
	assert(&c->channels[3] != NULL);
	c->channels[3].program_number = phdrs[0].pid;
	printf("%d", v->voice->midi);
	assert(v->voice->midi == 66);

	render(c);
	return 9;
}