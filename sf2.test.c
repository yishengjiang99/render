#include "sf2.c"
#include "shift.c"
#include "delay.c"
#include "stbl.c"
#include "libs/fft.c"

void cb(voice *data)
{
	voice *v = (voice *)data;
	zone_t *z = v->z;

	printf("\n %d %d %d  %d", (unsigned short)z->SampleId, (short)z->Attenuation, (short)z->ReverbSend, (unsigned char)v->midi);
}
int main(int argc, char const *argv[])
{
	readsf(fopen("file.sf2", "rb"));

	delay_data *delay = newDelay(4096);
	complex *cs = (complex *)malloc(sizeof(complex) * 4096);
	for (int i = 30; i < 80; i++)
	{
		int iter = 480;
		zone_t *z = get_sf(0, phdrs[0].bankId, i, 66);
		loop(z, delay->inputwave);
		delay_snds(delay);
		input_time_domain_floats(delay->input_size, delay->output, cs, stbl);
		//			FFT(cs, 12L, stbl);
		for (int i = 0; i < 2048; i++)
		{
			printf("\n %f %f ", (*cs).real, (*cs).imag);
		}
		iter--;
		FILE *ffp = popen("ffmpeg -f f32le -i pipe:0 -ac 1 -ar 44100 -f wav -|ffplay -i pipe:0", "w"); //(1, 48000);
	}

	// printNode(&start, &cb);
	// FILE *ffp = popen("ffmpeg -f f32le -i pipe:0 -ac 1 -ar 44100 -f wav -|ffplay -i pipe:0", "w"); //(1, 48000);
	// for (int i = 0; i < nshdrs; i++)
	// {
	// 	shdrcast *sh = (shdrcast *)(shdrs + i);
	// 	fwrite(sdta + sh->start, 4, sh->endloop - sh->start, ffp);
	// 	fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
	// 	fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
	// }

	/* code */
	return 0;
}
