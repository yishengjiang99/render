#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/fft.h"
#include "libs/wavetable_oscillator.h"
#include "runtime.h"

static double *stbl;
static FILE *fdout;
void simple(complex *cs, int N) {
  int n = log2(N);

  FFT(cs, n, stbl);
  bit_reverse(cs, n);
  printf("\noutput\n");

  for (int i = 0; i < n; i++) {
    printf("\n %d: %f \t %f", i, cs[i].real, cs[i].imag);
  }
}
void trucate(complex *cs, int N, int npartials) {
  int n = log2(N);
  FFT(cs, n, stbl);
  bit_reverse(cs, n);

  for (int i = 0; i < N; i++) {
    if (i < npartials && i > N - npartials) {
      cs[i].real = 0;
      cs[i].imag = 0;
    }
  }
  bit_reverse(cs, n);
  iFFT(cs, n, stbl);
}

int p1() {
  fdout = popen(
      "ffmpeg -y -f f32le -i pipe:0 -ac 1 -ar 48000 -f WAV sine2square.wav",
      "w");

  int N = 4096, n = log2(N);
  stbl = malloc(sizeof(double) * N / 4);
  complex c1s[N];
  sin_table(stbl, 10L);

  bzero(c1s, N * sizeof(complex));
  float ff[N];
  fread(ff, 4, N, fopen("01_Saw_4096.pcm", "rb"));
  wavetable_oscillator_data *oscref = init_oscillators();
#define spinFloat(oscref, ff, midi)                                        \
  {                                                                        \
    oscref->wave000 = ff;                                                  \
                                                                           \
    set_midi(0, midi);                                                     \
    for (int i = 0; i < 48000; i += oscref->samples_per_block) {           \
      oscref->fadeDim1 = 0.0;                                              \
      oscref->fadeDim1Increment = 1.0f / 48000.0f;                         \
      wavetable_1dimensional_oscillator(oscref);                           \
      fwrite(oscref->output_ptr, sizeof(float), oscref->samples_per_block, \
             fdout);                                                       \
    }                                                                      \
  }

  spinFloat(oscref, ff, 60);
  spinFloat(oscref, ff, 70);
  spinFloat(oscref, ff, 80);
  spinFloat(oscref, ff, 90);

  for (int i = 0; i < N; i++) {
    //	c1s[i].real =
    c1s[i].real = ff[i];
    c1s[i].imag = 0.0f;
  }
  FFT(c1s, n, stbl);
  bit_reverse(c1s, n);

  int npartials = 119;
  FFT(c1s, n, stbl);
  bit_reverse(c1s, n);
  for (int i = 0; i < n; i++) {
    printf("\nafter FFT %d:\t%f\t%f", i, c1s[i].real, c1s[i].imag);
  }
  for (int i = npartials; i < N / 2; i++) {
    c1s[i].real = 0;
    c1s[i].imag = 0;
  }
  for (int i = N / 2; i < N - npartials; i++) {
    c1s[i].real = 0;
    c1s[i].imag = 0;
  }
  for (int i = 0; i < N; i++) {
    printf("\nbefore iFFT %d:\t%f\t%f=", i, c1s[i].real, c1s[i].imag);
  }
  bit_reverse(c1s, n);
  iFFT(c1s, n, stbl);
  for (int i = 0; i < n; i++) {
    printf("\nafter iFFT %d:\t%f\t%f", i, c1s[i].real, c1s[i].imag);
  }
  bzero(ff, sizeof(ff));
  for (int i = 0; i < N; i++) {
    ff[i] = 20.0f * (float)c1s[i].real;  // = ff[i];
  }
  for (int i = 0; i < n; i++) {
    printf("\n %d: %f \t %f", i, c1s[i].real, c1s[i].imag);
  }

  spinFloat(oscref, ff, 60);
  spinFloat(oscref, ff, 70);
  spinFloat(oscref, ff, 80);
  spinFloat(oscref, ff, 90);
  system("ffplay sine2square.wav");

  return 1;
}

int main() {
  init_ctx();
  readsf(fopen("GeneralUserGS.sf2", "rb"));
  setProgram(0, 0);
  noteOn(0, 66, 55, 0);

  render(g_ctx);
  render(g_ctx);
  render(g_ctx);
  render(g_ctx);

  int N = 4096, n = log2(N);
  stbl = malloc(sizeof(double) * N / 4);
  complex c1s[N];
  sin_table(stbl, 10);
  bzero(c1s, N * sizeof(complex));
  float ff[N];
  for (int i = 0; i < N; i++) {
    //	c1s[i].real =
    c1s[i].real = g_ctx->outputbuffer[2 * i];  // stero signal to mono
    c1s[i].imag = 0.0f;
    printf("\n%f,", c1s[i].real);
  }
  FFT(c1s, n, stbl);
  bit_reverse(c1s, n);
  FILE *of = fopen("tesxt.pcm", "wb");
  fwrite(c1s, sizeof(complex), N, of);
  return 0;
}