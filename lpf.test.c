#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "lpf.c"
#include "call_ffp.c"
#include "runtime.c"
int main()
{
	readsf(fopen("GeneralUserGS.sf2", "rb"));
	init_ctx();
	ctx_t *ctx = g_ctx;
	zone_t z;
	shdrcast sh;
	z.OverrideRootKey = -1;
	z.FineTune = 100;
	sh.sampleRate = 24000.0f;
	sh.originalPitch = 60;
	z.OverrideRootKey = -1;
	z.OverrideRootKey = -1;
	sh.originalPitch = 58;

	z.FineTune = 10;
	printf("%f,%f", calcratio(&z, &sh, 55), powf(2.0f, (5500.0f - 6100.0f) / 1200.0f) * sh.sampleRate / ctx->sampleRate);
	sh.sampleRate = g_ctx->sampleRate;
	printf("\n%f,%f", calcratio(&z, &sh, 58), powf(2.0f, -10.0f / 1200.0f));
	PresetZones pz = findPresetByName("Flute");
	assert(pz.hdr.pid != 0);

	// FILE *sawtooth = fopen("sawtooth.pcm", "rb");
	// float *buf = (float *)malloc(sizeof(float) * 48000 * 1);
	// fread(buf, 4, 48000 * 1, sawtooth);
	// float *afterbb = (float *)malloc(sizeof(float) * 48000);

	// lpf *filter = malloc(sizeof(lpf));

	// float input;
	// newLpf(filter, 331.0f, 48000.0f);
	// for (int i = 0; i < 48000; i++)
	// {
	// 	filter->input = *(buf + i);
	// 	*(afterbb + i) = process_input(filter, *(buf + i));
	// }
	// FILE *l = popen("ffplay -t 1 -ac 1 -ar 48k -f f32le -i pipe:0", "w");

	// fwrite(afterbb, 4, 48000, l);
	// pclose(l);

	//	system("ffmpeg -ac 1 -f f32le -i sawtooth.pcm -filter:a lowpass=f=330 -f f32le -|ffplay -ac 1 -f f32le -i pipe:0");
	//lpf=frequency:220");
}
