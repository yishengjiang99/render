#include <math.h>
#include <stdio.h>
#define DBFULLSCAlE 960.0f
#define PI
static float panLUT[1000];
static float p2over1200LUT[1200];
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
static float centdbLUT[962];
static float centdblut(int x) {
  if (x < 0) x = 0;
  if (x > 960) x = 960;

  return centdbLUT[x];
}
void initLUTs() {
  FILE *f = fopen("luts.c", "w");
  fprintf(f, "#define LUT 1\n float p2over1200LUT[1202] = {");
  for (int i = 0; i <= 1200; i++) {
    p2over1200LUT[i] = powf(2.0f, (float)i / 1200.0f);
    fprintf(f, "%ff,", powf(2.0f, (float)i / 1200.0f));
  }
  fprintf(f, "2.0f};\n");
  fprintf(f, "float centdbLUT[961] = {");
  for (int i = 0; i < 960; i++) {
    centdbLUT[i] = powf(10.0f, (float)i / -200.0);
    fprintf(f, "\n%ff,", powf(10.0f, (float)i / -200.0));
  }
  fprintf(f, "0.0f};");

  fprintf(f, "\n float midiCB[129] = { 960.f, ");

  for (int n = 1; n < 128; n++) {
    fprintf(f, "%ff,", (-200.0) * logf(n / 127.0f) / M_LN10);
  }
  fprintf(f, "0.0f};");

  fprintf(f, "\n\nfloat velCB[129] = { 960.f, ");

  for (int n = 1; n < 128; n++) {
    fprintf(f, "\n%ff,", -200.0 * logf(n * n / 127.0f / 127.0f) / M_LN10);
  }
  fprintf(f, "0.0f};");

  fprintf(f, "\nfloat pan[1001] = {");

  for (int n = 1; n < 1001; n++) {
    fprintf(f, "%ff,", sinf((float)(n - 500.0f) / 1000.0f * M_2_PI));
  }
  fprintf(f, "0.0f};");

  fprintf(f, "float cent2hz[1201]={");
  for (int i = 0; i < 1200; i++) {
    fprintf(f, "%ff,", 8.176f * powf(2.f, i / 1200.f));
  }
  fprintf(f, "%ff};", 8.176f * 2);
}

int main() { initLUTs(); }
