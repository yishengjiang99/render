

#include "fft-samples.h"

#include <strings.h>

#include "includes/biquad.h"
#include "includes/fft.h"
#include "includes/wavetable_oscillator.h"
#include "macros.h"
#include "includes/runtime.h"
#include "includes/sf2.h"

#ifndef SAMPLE_RATE  // the other one is the rendering engine samp rate
#define SAMPLE_RATE 48000

#endif
void init_detailed_wavtables(int pid, int bankid);
void render_and_fft(voice* v, complex* c, double* stbl);
static double stbl[FFTBINS / 4];

int main(int argc, char** argv) {
  sin_table(stbl, log2(FFTBINS));

  init_oscillators();

  char* sf2file = argc > 1 ? argv[1] : "file.sf2";
  char outfile[1024];
  double sinetable[FFTBINS << 2];
  memcpy(sinetable, stbl, (FFTBINS << 2) * sizeof(double));
  readsf(sf2file);
  init_ctx();
  for(int i=0;i<nphdrs;i++){
    int pid = phdrs[i].pid;
    int bid = phdrs[i].bankId;
    init_detailed_wavtables(pid, bid);

  }
  return 1;
}
void init_detailed_wavtables(int pid, int bankid) {
  int cid = bankid == 128 ? 9 : 0;
  setProgram(cid, pid);
  PresetZones presetZones = g_ctx->channels[cid].pzset;
  complex c[FFTBINS];
  wavetable_sequence sequences[presetZones.npresets];
  
  for(int i=0; i<presetZones.npresets;i++){
    zone_t z =presetZones.zones[i];
    shdrcast* sh = (shdrcast*)(shdrs + z.SampleId);
    g_ctx->sampleRate = sh->sampleRate;
    voice *v = newVoice(&z, z.KeyRange.hi, z.VelRange.hi, cid);
    render_and_fft(v, c, stbl);
  }
}

void render_and_fft(voice* v, complex* c, double* stbl) {
  bzero(c, sizeof(complex) * FFTBINS);
  channel_t dummy;
  loop(v, g_ctx->outputbuffer, dummy);
  for (int i = 0; i < FFTBINS; i++) {
    c[i].real = g_ctx->outputbuffer[i * 2];
    c[i].imag = 0.0f;
  }
  FFT(c, log2(FFTBINS), stbl);
  bit_reverse(c, log2(FFTBINS));
  int npartials = 11;
  for (int i = 0; i < FFTBINS; i++) {
     printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);

    if (i > npartials && i <= FFTBINS / 2) {
      c[i].real = 0.0f;
      c[i].imag = 0.0f;
    }
    if (i > FFTBINS / 2 && i < FFTBINS - npartials - 1) {
      c[i].real = 0.0f;
      c[i].imag = 0.0f;
    }
  }

  bit_reverse(c, log2(FFTBINS));
  iFFT(c, log2(FFTBINS), stbl);
  for (int i = 0; i < FFTBINS; i++) {
    printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);
  
  }
}
