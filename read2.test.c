#include "sf2.c"

void iter_callback(void *data)
{
	voice *v = (voice *)data;
	zone_t *z = v->z;

	printf("\n %d %d %d  %d", (unsigned short)z->SampleId, (short)z->Attenuation, (short)z->ReverbSend, (unsigned char)v->midi);
}
int main(int argc, char const *argv[])
{
	readsf(fopen("file.sf2", "rb"));
	node *start = 0;
	get_sf(0, 0, 55, 33, &start, 55);
	get_sf(0, 0, 33, 22, &start, 33);
	get_sf(0, 0, 55, 22, &start, 22);
	get_sf(0, 0, 22, 22, &start, 33);

	printNode(&start, &iter_callback);
	FILE *ffp = popen("ffmpeg -f f32le -i pipe:0 -ac 1 -ar 44100 -f wav -|ffplay -i pipe:0", "w"); //(1, 48000);
	for (int i = 0; i < nshdrs; i++)
	{
		shdrcast *sh = (shdrcast *)(shdrs + i);
		fwrite(sdta + sh->start, 4, sh->endloop - sh->start, ffp);
		fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
		fwrite(sdta + sh->startloop, 4, sh->endloop - sh->startloop, ffp);
	}

	/* code */
	return 0;
}
