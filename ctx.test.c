#include "ctx.c"
#include <assert.h>
#include "call_ffp.c"
#include <termios.h>
void cb(voice *v)
{

	printf("%f %f %d %d\n", v->ratio, v->ampvol->db_attenuate, v->startloop, v->endloop);
}
int main()
{
	// initLUTs();
	readsf(fopen("file.sf2", "rb"));
	ctx_t *c = init_ctx();
	c->outputFD = ffp(2, 44100);
	c->channels[0].program_number = 0;

	noteOn(c, 0, 33, 55, 5);

	render_fordr(c, 0.25);
	noteOff(c, 0, 55);
	noteOn(c, 0, 77, 66, c->currentFrame + 1);
	render_fordr(c, 0.25);
	noteOff(c, 0, 77);
	noteOn(c, 0, 44, 77, c->currentFrame + 1);
	render_fordr(c, 4.5);
	return 9;
}