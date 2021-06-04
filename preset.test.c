#include "sf2.c"
#include <assert.h>
#include "call_ffp.c"
#include "runtime.c"
#include "libs/fft.c"
#include "stbl.c"

int main()
{
	init_ctx();
	g_ctx->samples_per_frame = 4096;
	readsf(fopen("file.sf2", "rb"));
	PresetZones pz = findPresetByName("Trumpet");
	printf("%d", pz.hdr.pid);
	for (int i = 0; i < pz.npresets; i++)
	{
		printf("\n%d", i);
		voice *v = newVoice(pz.zones + i, 55, 33);
		zone_t *z = pz.zones + i;
		printf("\nkeyrange %d %d", z->KeyRange.lo & 0x7f, z->KeyRange.hi & 0x7f);
		printf("\nvange %x %x", z->VelRange.lo, z->VelRange.hi);
		float ff[4096 * 2];
		loop(v, ff);
		char fn[1024];
		complex cs[4096];
		sprintf(fn, "%s_key_%hu_%hu_vel_%hu_%hu_lpf_fc_%d.wav", pz.hdr.name, z->KeyRange.lo, z->KeyRange.hi, z->VelRange.lo, z->VelRange.hi, z->FilterFc);
		for (int j = 0; j < 4096; j++)
		{
			cs[j].real = ff[j * 2];
			cs[j].imag = 0.0f;
		}
		FFT(cs, 12, stbl);
		bit_reverse(cs, 12);
		FILE *fftres = formatpcm("wav", fn);

		truncate(fftres, 6);
		bit_reverse(cs, 12);

		fwrite(cs, sizeof(complex), 4096, fftres);
	}

	//printz(h); //0, 0, 55, 33h); ///p, 0);
}