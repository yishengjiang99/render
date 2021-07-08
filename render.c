#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/wavetable_oscillator.c"
#include "sf2/sf2.c"
#define MODULO_one BIT32_NORMALIZATION
#define zerogain pow(10.0f, -1440.0f)

typedef struct {
  int start, startloop, endloop;
  int pos;
  float frac, ratio;
  float *sample;
  float *output;
} sampler_iterater;

float hermite4(float frac_pos, float xm1, float x0, float x1, float x2) {
  const float c = (x1 - xm1) * 0.5f;
  const float v = x0 - x1;
  const float w = c + v;
  const float a = w + v + (x2 - x0) * 0.5f;
  const float b_neg = w + a;

  return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

int main(int argc, char **argv) {
  char *readff = argc > 1 ? argv[1] : "file.sf2";
  char *filename = argc > 2 ? argv[2] : "file.html";
  readsf(readff);

  zone_t *zone = filterForZone(findByPid(60, 0), 60, 122);
  shdrcast *sh = (shdrcast *)(shdrs + zone->SampleId);
}

#define frequency(midiPitch) \
  (440.0f * powf(2.0f, (float)(midiPitch - 69.0f) / 12.0f))

text(sampler_iterater *sample) {
  // FILE *Fd = fopen(sh->name, "wb");
  // fwrite(wavetables, 4, 148000, Fd);
  wavetable_oscillator_data *osc = init_oscillators();
  float *wavetable_ref = sample->output;

  osc->wave000 = wavetable_ref;
  osc->wave001 = wavetable_ref += 1 * WAVETABLE_SIZE;
  osc->fadeDim1 = 0.0f;
  osc->fadeDim1Increment = 1 / 1.0f / WAVETABLE_SIZE;
  int midi = 55;
  osc->phaseIncrement = powf(2.0, (midi - 69) / 12.0f) * 440.0f *
                        BIT32_NORMALIZATION / SAMPLE_RATE;
  // FILE *output = formatpcm("mp3", "note.mp3");
  FILE *output = ffp(1, 48000);
  osc->wave010 = sawtooth;
  osc->wave011 = sinewave;

  osc->fadeDim2 = .1;

  for (int i = 0; i <= 48000 - SAMPLE_BLOCKSIZE; i += SAMPLE_BLOCKSIZE) {
    wavetable_2dimensional_oscillator(osc);
    fwrite(osc->output_ptr, 4, SAMPLE_BLOCKSIZE, output);

    if (osc->fadeDim2 > 1.0f) {
      osc->fadeDim2Increment = -80 / SAMPLE_RATE;
    } else if (osc->fadeDim2 <= -1.0f) {
      osc->fadeDim2Increment = -80 / SAMPLE_RATE;
    }
    if (osc->fadeDim1 >= 1.0f) {
      osc->wave000 = wavetable_ref += 1 * WAVETABLE_SIZE;
      osc->fadeDim1Increment *= -1;
    } else if (osc->fadeDim1 <= 0.0f) {
      osc->wave001 = wavetable_ref += 1 * WAVETABLE_SIZE;
      osc->fadeDim1Increment *= -1;
    }
  }
}