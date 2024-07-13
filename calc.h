#include "p1200.h"
#ifndef CALC_H
#define CALC_H

#define log_2_10 3.321928094f
#define bit23_normalize 1.000f / 0x7fffff
#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100.0f
#endif
#define SAMPLE_BLOCK 128
#define BLOCKS_PER_SECOND SAMPLE_RATE / SAMPLE_BLOCK

#define clamp(val, min, max) val > max ? max : val < min ? min : val

double timecent2second(short tc) {
  if (tc < 0) return 1.0f / timecent2second(-1 * tc);
  if (tc > 1200) return 2.0f * timecent2second(tc - 1200.0f);
  return p2over1200Arr[tc];
}

double timecent2hertz(short tc) { return 8.176f * timecent2second(tc); }

int timecent2sample(short tc) {
  return (int)(timecent2second(tc) * SAMPLE_RATE);
}

float applyCentible(float signal, short centdb) {
  if (centdb > 0) return signal;
  if (centdb <= -1240) return 0.0f;
  return (float)signal * p10over200[centdb + 960];
}

static inline short sf2midiPan(short sf2pan) {
  if (sf2pan > 500) return 128;
  if (sf2pan < -500) return 1;
  return (short)64 + sf2pan / 500 * 64;
}
double midi_volume_log10(int val) {
  if (val < 0) return 1440;
  if (val > 128) return 0;
  return midi_log_10[val];
}
#endif