#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fft-samples.h"
#include "libs/wavetable_oscillator.c"

#define noteOn(ch, note, vel)       \
  {                                 \
    midi_put_c(NOTE_ON | (char)ch); \
    midi_put_c((char)note & 0x7f);  \
    midi_put_c((char)vel & 0x7f);   \
    midi_put_c(0);                  \
  }

#define noteOff(ch, note, vel)       \
  {                                  \
    midi_put_c(NOTE_OFF | (char)ch); \
    midi_put_c((char)note & 0x7f);   \
    midi_put_c((char)vel & 0x7f);    \
    midi_put_c(0);                   \
  }

int main() {
  wavetable_oscillator_data* oscillator = init_oscillators();
  printf("\n%f %f", oscillator[0].fadeDim1Increment, oscillator[0].fadeDim1);
  oscillator[0].fadeDim1 += 0.001f;
  printf("\n%f %f", oscillator[0].fadeDim1Increment, oscillator[0].fadeDim1);
  return 1;
}