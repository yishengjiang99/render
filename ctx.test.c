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
	zone_t *z = get_sf(0 & 0x7f, 0 & 0x80, 46, 120);

	ctx_t *c = init_ctx();
	c->mastVol = 0.5;
	//c->outputFD = wavepic("s.png");
	c->outputFD = ffp(2, 48000);

	c->channels[0]
			.program_number = phdrs[1].pid;
	int notes[] = {55, 67, 55, 67, 55, 67, 72, 60, 72, 55, 69};
	for (int i = 0; i < 10; i++)
	{

		noteOn(c, 0, notes[i], 66, 5);

		render_fordr(c, 0.25);
		noteOff(c, 0, 55);
		noteOn(c, 0, 77, 66, c->currentFrame + 1);
		render_fordr(c, 0.25);
		noteOff(c, 0, 77);
		noteOn(c, 0, 44, 77, c->currentFrame + 1);
		render_fordr(c, 4.5);
		return 9;
	}