#include "LFO.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "calc.h"
/*
   VIBLFOTOPITCH: 40.00
      FILTERQ: -110.00
      MODLFOTOVOL: 20.00
      CHORUSSEND: 300.00
      REVERBSEND: 40.00
      MODLFOFREQ: -884.00
      VIBLFOFREQ: -1200.00
      VOLENVDELAY: -3986.00
      VOLENVATTACK: -3986.00
      VOLENVHOLD: -7973.00
      VOLENVDECAY: 3600.00
      VOLENVSUSTAIN: 960.00
      VOLENVRELEASE: 1200.00
      */
#define rollprint(n, block)                                         \
  {                                                                 \
    for (int i = 0; i < n; i += block) {                            \
      printf("\n%f", LFO_roll(lfo, block));                         \
      printf("\t %4x", LFO_roll(lfo, block) / modulo_s16f_inverse); \
    }                                                               \
  }

int main() {
  LFO lfo[2];
  lfo->delay = 1200;
  lfo_set_frequency(lfo, timecent2hertz(-1200));
  LFO_roll(lfo, 1);

  lfo_set_frequency(lfo, timecent2hertz(-1200));
  rollprint(48000, 1);
}