#include <math.h>
#include <stdio.h>
#include "libs/wavetable-oscillator.c"
#include "libs/fft.c"

#include <assert.h>
#include "sf2.c"
#include "call_ffp.c"
#include "shift.c"
#include "delay.c"

#include "stbl.c"
int main()
{
	init_oscillators();
	readsf(fopen("file.sf2", "r"));
	shdrcast *sh = (shdrcast *)(shdrs + 3);

	// for (int i = 0; i < nshdrs; i++)
	// {
	// 	shdrcast *sh = (shdrcast *)(shdrs + i);
	// 	fwrite(sdta + sh->start, 4, sh->endloop - sh->start, ffp);
	// 	fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
	// 	fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
	// }
	int midi = 55, outputs = 0;
	zone_t *z = get_sf(0, phdrs[0].bankId, midi, 66);

	voice *v = newVoice(z, 55, 99);

	printf("%f", v->ratio, v->ampvol->db_attenuate);

	delay_data *delay = newDelay();
	float *tmp = malloc(128 * sizeof(float));
	complex *cs = (complex *)malloc(sizeof(long) * 4096);
	while (outputs < 48000)

	{
		loop(v, tmp);

		memcpy(write_ptr(delay), tmp, 128 * sizeof(float)); // write_ptr(delay));
		fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);

		advance_write_ptr(delay);
		float *output = read_ptr(delay);
		outputs += delay_input_size + 0.5f;

		if (delay->write_offset == 0)
		{
			input_time_domain_floats(4096, delay->delaybuffer, cs, &(stbl[0]));
		}
	}
	//fclose(ff);
}