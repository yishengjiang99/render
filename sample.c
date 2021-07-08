
typedef unsigned int uint32_t;
#define wavetable_size 4096
typedef struct _iterate {
  uint32_t startloop, endloop;
  uint32_t nsamples_output;
  uint32_t pos;
  float ratio, frac;
  float *output;
  float *sample;
} sampler_iterator;

static sampler_iterator iterates[1];
static float output[wavetable_size];
static float sample[wavetable_size * 10];

sampler_iterator *init() {
  iterates->output = output;
  iterates->sample = sample;
  return iterates;
}

float hermite4(float frac_pos, float xm1, float x0, float x1, float x2) {
  const float c = (x1 - xm1) * 0.5f;
  const float v = x0 - x1;
  const float w = c + v;
  const float a = w + v + (x2 - x0) * 0.5f;
  const float b_neg = w + a;
  return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

void upsample_wave_table(sampler_iterator *v) {
  v->frac = 0.0f;
  v->pos = 0;
  for (int i = 0; i < v->nsamples_output; i++) {
    float fm1 = *(v->sample + v->pos - 1);
    float f1 = *(v->sample + v->pos);
    float f2 = *(v->sample + v->pos + 1);
    float f3 = *(v->sample + v->pos + 2);
    v->output[i] = hermite4(v->frac, fm1, f1, f2, f3);
    v->frac += v->ratio;
    if (v->frac >= 1.0f) {
      v->frac--;
      v->pos++;
    }
    if (v->pos > v->endloop) {
      v->pos -= (v->endloop - v->startloop);
    }
  }
}
