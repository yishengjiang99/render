#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fft-samples.h"
#include "glue.c"
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
  midi_put_c(PROGRAM_CHANGE | 0x02);
  midi_put_c(clarinet);
  midi_put_c(0);
  midi_put_c(0);
  midi_put_c(PROGRAM_CHANGE | 0x03);
  midi_put_c(string_ens);
  midi_put_c(0);
  midi_put_c(0);
  midi_put_c(PROGRAM_CHANGE | 0x04);
  midi_put_c(string_ens);
  midi_put_c(0);
  midi_put_c(0);
  midi_put_c(PROGRAM_CHANGE | 0x05);
  midi_put_c(string_ens);
  midi_put_c(0);
  midi_put_c(0);
  midi_put_c(PROGRAM_CHANGE | 0x06);
  midi_put_c(string_ens);
  midi_put_c(0);
  midi_put_c(0);
  midi_put_c(PROGRAM_CHANGE | 0x07);
  midi_put_c(string_ens);
  midi_put_c(0);
  midi_put_c(0);
  float ob[128];
  FILE* ff = popen("ffplay -f f32le -ac 1 -i pipe:0", "w");

  noteOn(0x02, 67, 116);
  noteOn(0x03, 67, 116);
  noteOn(0x04, 67, 116);
  noteOn(0x05, 55, 116);
  noteOn(0x06, 43, 116);
  noteOn(0x07, 43, 116);

  for (int i = 0; i < 48000 * 0.2; i += 128) {
    spin_oscillator_event_loop(ob, 128);
    fwrite(ob, sizeof(float), 128, ff);
  }

  noteOff(0x02, 67, 116);
  noteOff(0x03, 67, 116);
  noteOff(0x04, 67, 116);
  noteOff(0x05, 55, 116);
  noteOff(0x06, 43, 116);
  noteOff(0x07, 43, 116);
  for (int i = 0; i < 48000 * 0.10; i += 128) {
    spin_oscillator_event_loop(ob, 128);
    fwrite(ob, sizeof(float), 128, ff);
  }

  return 1;
}