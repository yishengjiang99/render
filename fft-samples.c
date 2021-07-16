

#include "fft-samples.h"

#include <strings.h>

#include "call_ffp.c"
#include "libs/biquad.c"
#include "libs/fft.c"
#include "libs/wavetable_oscillator.c"
#include "macros.h"
#include "runtime.c"
#include "sf2.c"
#include "sf2.h"
#ifndef stbl
#include "stbl.c"
#endif
#ifndef SAMPLE_RATE  // the other one is the rendering engine samp rate
#define SAMPLE_RATE 48000

#endif
static wavetable_set table_set[1];
void init_wavetabe_set(wavetable_set* tset, int pid, int bankid);
void render_and_fft(voice* v, complex* c, double* stbl, float* destination);

int main(int argc, char** argv) {
  // char buf[sizeof(wavetable_set)];
  // bmpheader(buf, sizeof(wavetable_set), nkeys * nvels * 4 * 2);
  // fwrite(buf, sizeof(wavetable_set), 1, fopen("bmphead.dat", "wb"));
  // openurl("https://dsp.grepawk.com/render/pages/");
  init_oscillators();
  int pid = argc > 1 ? atoi(argv[1]) : 0;
  int bid = argc > 2 ? atoi(argv[2]) : 0;

  char* sf2file = argc > 3 ? argv[3] : "file.sf2";
  char outfile[1024];
  int N = 4096, n = 12;
  double sinetable[N / 4];
  sin_table(sinetable, 12);
  readsf(sf2file);
  complex c[FFTBINS];
  init_ctx();
  // g_ctx->outputFD = ffp(1, 48000);

  init_wavetabe_set(table_set, pid, bid);
  sprintf(outfile, "pages/%d_%d.dat", pid, bid);
  fwrite(table_set, sizeof(table_set), 1, fopen(outfile, "wb"));
}
void init_wavetabe_set(wavetable_set* tset, int pid, int bankid) {
  int cid = bankid == 128 ? 9 : 0;
  setProgram(cid, pid);
  complex c[FFTBINS];
  for (int i = 0; i < nkeys; i++) {
    for (int j = 0; j < nvels; j++) {
      int midi =
          20 + i * 80.0f / (nkeys * 1.0f);  // i = (midi-20)*(nkeys*1.0f)/80.0f
      int vel = j * 120.9f / (float)nvels;  // j=(float)vel/120.0f*nvels
      noteOn(cid, midi, vel, 0);
      voice* v1 = g_ctx->channels[cid].voices;
      if (!v1) {
        fprintf(stderr, "\n-v1 not found at %d %d %d %d", pid, bankid, i, j);
        continue;
      }
      adsr_t* ampvol = v1->ampvol;
      adsr_t* modvol = v1->moddvol;

      shdrcast* sh = (shdrcast*)(shdrs + v1->z->SampleId);
      tset->envelope[i][j][ATT] = v1->z->VolEnvAttack;
      tset->envelope[i][j][DEC] = v1->z->VolEnvDecay;  //+ v1->z->VolEnvHold;
      tset->envelope[i][j][REL] = v1->z->VolEnvRelease;
      tset->envelope[i][j][SUS] = v1->z->VolEnvSustain;

      render_and_fft(v1, c, stbl, tset->init_att[i][j]);
      ampvol->db_attenuate = 0;
      modvol->db_attenuate = 0;
      ampvol->att_steps = 0;
      modvol->att_steps = 0;
      render_and_fft(v1, c, stbl, tset->eg_peak[i][j]);
      v1->pos = v1->startloop;
      render_and_fft(v1, c, stbl, tset->loop[i][j]);
      noteOff(cid, midi);
      ampvol->db_attenuate = ampvol->sustain;
      modvol->db_attenuate = modvol->sustain;
      g_ctx->channels[cid].voices = NULL;
      render_and_fft(v1, c, stbl, tset->decay[i][j]);
      oscillator->wave000 = tset->loop[i][j];
      set_midi(0, 60);
      oscillator->fadeDim1 = 0.0f;
      oscillator->fadeDim1Increment = 1.0f / SAMPLE_RATE * 23.0f;
      // for (int i = 0; i < 48000; i += SAMPLE_BLOCKSIZE) {
      //   wavetable_1dimensional_oscillator(oscillator);
      //   fwrite(oscillator->output_ptr, 4, 1, g_ctx->outputFD);
      // }
      if (v1 != NULL) free(v1);
    }
  }
}
void render_and_fft(voice* v, complex* c, double* stbl, float* destination) {
  bzero(c, sizeof(complex) * FFTBINS);
  bzero(destination, sizeof(float) * FFTBINS);
  channel_t dummy;
  loop(v, g_ctx->outputbuffer, dummy);
  for (int i = 0; i < FFTBINS; i++) {
    c[i].real = g_ctx->outputbuffer[i * 2];
    c[i].imag = 0.0f;
  }
  FFT(c, log2(FFTBINS), stbl);
  bit_reverse(c, log2(FFTBINS));
  int npartials = 600;
  for (int i = 0; i < FFTBINS; i++) {
    // printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);

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
    // printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);
    destination[i] = (float)c[i].imag;
  }
}
