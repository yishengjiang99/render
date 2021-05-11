#include "ctx.c"
#include <assert.h>
#include "call_ffp.c"
void cb(voice *v)
{
	printf("%f %f %d %d\n", v->ratio, v->ampvol->db_attenuate, v->startloop, v->endloop);
}
int main()
{
	initLUTs();
	readsf(fopen("file.sf2", "rb"));
	ctx_t *c = init_ctx(ffp(2, 44100));

	c->channels[0].program_number = 55;
	noteOn(c, 0, 55, 55);
	printNode(&c->voices, &cb);

	return 9;
}