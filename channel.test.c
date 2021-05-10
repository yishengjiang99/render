#include "channel.c"
#include <assert.h>

int main()
{
	initLUTs();
	readsf(fopen("file.sf2", "rb"));
	ctx_t *c = init_ctx();
	assert(c != 0);
	assert(&c->channels[3] != NULL);
	c->channels[3].program_number = phdrs[0].pid;

	noteOn(c, 3, 35, 37);
	printf("\n%d\n", c->channels[3].voices[0].midi);
	assert(c->channels[3].voices[0].midi == 35);

	assert(c->channels[3].voices[0].ampvol->att_steps > 2);
}