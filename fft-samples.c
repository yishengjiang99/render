#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/fft.h"
#include "libs/wavetable_oscillator.h"
#define output_sampleRate 480000
#define dspbuffersize 256
#include "runtime.c"
#include "sf2.c"
#include "stbl.c"
#define FFTBINS 256
#define sampleVoice(v, c, fo)                            \
  {                                                      \
    bzero(g_ctx->outputbuffer, sizeof(float) * FFTBINS); \
    loop(v, g_ctx->outputbuffer, g_ctx->channels[0]);    \
    bzero(c, FFTBINS * sizeof(complex));                 \
    for (int k = 0; k < FFTBINS; k++) {                  \
      c[k].real = g_ctx->outputbuffer[k];                \
      c[k].imag = 0.0f;                                  \
    }                                                    \
    FFT(c, log2(FFTBINS), stbl);                         \
    fwrite(c, sizeof(complex), FFTBINS, fo);             \
    fflush(fo);                                          \
  }

int main() {
  double sinetable[FFTBINS << 2];
  memcpy(sinetable, stbl, (FFTBINS << 2) * sizeof(double));
  char *file = "FluidR3_GM.sf2";
  readsf(fopen(file, "rb"));
  printf("%d %d", nphdrs, nshdrs);
  complex c[FFTBINS];
  init_ctx();

  for (int i = 0; i < nphdrs; i++) {
    if (phdrs[i].bankId != 0) continue;
    char fname[1024];
    sprintf(fname, "./rr/%d_%d_%s.riff", phdrs[i].pid, phdrs[i].bankId,
            phdrs[i].name);
    FILE *fo = fopen(fname, "wb");

    PresetZones pz = findByPid(phdrs[i].pid, phdrs[i].bankId);
    zone_t *zones = pz.zones;
    // fwrite(&pz.hdr, sizeof(phdr), 1, fo);
    //    fwrite(&pz.npresets, sizeof(int), 1, fo);

    for (int j = 0; j < pz.npresets - 1; j++) {
      // fwrite(zones, sizeof(zone_t), 1, fo);

      shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);
      short *attrs = (short *)zones;
      voice *v = newVoice(zones, zones->KeyRange.lo, zones->VelRange.hi, 0);

      sampleVoice(v, c, fo);

      // set vol envelope to peak
      v->ampvol->att_steps = 50;
      v->ampvol->db_attenuate = 1;
      sampleVoice(v, c, fo);
      zones++;
    }
    fclose(fo);
  }

  return 0;
}