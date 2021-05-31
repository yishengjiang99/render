#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "lpf.c"
#include "call_ffp.c"
int main()
{
	FILE *before = fopen("sawtooth.pcm", "rb");
	lpf *filter = malloc(sizeof(lpf));
	FILE *after = fopen("after.pcm", "wb");

	float input;
	newLpf(filter, 331.0f, 48000.0f);

	fputc(process_input(filter, (before)), after);

	float *buf = (float *)malloc(sizeof(float) * 48000 * 1);
	fread(buf, 4, 48000 * 1, stf);
	//	fwrite(buf, 4, 48000, playf);
	float *after = (float *)malloc(sizeof(float) * 48000);

	for (int i = 0; i < 48000; i++)
	{
		filter->input = *(buf + i);
		process_input(filter, fgetc(stf));
		*(after + i) = filter->output;
	}
	fwrite(after, 4, 48000, playf);
	system("ffplay -t 1 -ac 1 -ar 48k -f f32le -i after.pcm");
	system("ffmpeg -ac 1 -f f32le -i sawtooth.pcm -filter:a lowpass=f=330 -f f32le -|ffplay -ac 1 -f f32le -i pipe:0");
	//lpf=frequency:220");
}
