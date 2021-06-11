#ifndef lut
#define lut
#include "luts.c"
static inline float p2over1200(float x) {
  if (x < -12000) return 0;
  if (x < 0)
    return 1.f / p2over1200(-x);
  else if (x > 1200.0f) {
    return 2 * p2over1200(x - 1200.0f);
  } else {
    return p2over1200LUT[(unsigned short)(x)];
  }
}
static inline float centdblut(float x) {
  if (x < 0) x = 0.0f;
  if (x > 960) x = 960.0f;

  return powf(10.0f, x / -200.0f);
}
static inline float midiCBlut(int midi) {
  if (midi < 0) return -960.0f;
  if (midi > 960) return 0.0f;
  return midiCB[midi];
}
static inline uint32_t timecent2steps(short tc, uint32_t samplerate) {
  return p2over1200(tc) * samplerate;
}
static inline float centtone2freq(unsigned short ct) {
  return p2over1200(ct - 6900) * 440.0f;
}
static inline float ct2relativePitch(short ct) { return p2over1200(ct); }

static inline float panLeftLUT(short Pan) {
  if (Pan < -500) {
    return 1;
  }
  if (Pan > 500) {
    return 0.0f;
  } else {
    return 0.5f + pan[Pan + 500];
  }
}
#endif

short attrs[240];