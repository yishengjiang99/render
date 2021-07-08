#include "biquad/biquad.c"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "call_ffp.c"
#include "libs/wavetable_oscillator.c"
#include "sf2/sf2.c"
#define MODULO_one BIT32_NORMALIZATION

typedef struct {
  uint32_t phase;
  int32_t phaseShift;
  float *wavetable;
  int pos;
  float frac, ratio;
} voice_t;
float hermite4(float frac_pos, float xm1, float x0, float x1, float x2) {
  const float c = (x1 - xm1) * 0.5f;
  const float v = x0 - x1;
  const float w = c + v;
  const float a = w + v + (x2 - x0) * 0.5f;
  const float b_neg = w + a;

  return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

// adsr_t *newEnvelope(short centDelay, short centAtt, short centHold,
//                     short centRelease, short centDecay, short sustain,
//                     unsigned short attenutate, int sampleRate) {
//   adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
//   env->delay_steps = powf(2.0f, (float)centDelay / 1200.0f) * sampleRate;
//   env->hold_steps = powf(2.0f, (float)centHold / 1200.0f) * sampleRate;

//   env->att_steps = powf(2.0f, (float)centAtt / 1200.0f) * sampleRate;
//   env->decay_steps = powf(2.0f, (float)centDecay / 1200.0f) * sampleRate;
//   env->release_steps = powf(2.0f, (float)centRelease / 1200.0f) * sampleRate;
//   env->att_rate = -960.0f / env->att_steps;
//   env->decay_rate = ((float)1.0f * sustain) / (float)env->decay_steps;
//   env->release_rate = (float)960.0f / ((float)env->release_steps);
//   env->db_attenuate = 959.0f;

//   return env;
// }
// float envShift(adsr_t *env) {
//   if (env->delay_steps-- > 0) {
//   }

//   else if (env->att_steps > 0) {
//     env->att_steps--;
//     env->db_attenuate += env->att_rate;
//   } else if (env->hold_steps-- > 0) {
//   } else if (env->decay_steps > 0) {
//     env->decay_steps--;
//     env->release_steps--;

//     env->db_attenuate += env->decay_rate;
//   } else if (env->release_steps > 0) {
//     env->release_steps--;
//     env->db_attenuate += env->release_rate;
//   }
//   // printf("%f", env->db_attenuate);
//   if (env->db_attenuate > 960) {
//     if (env->att_steps <= 0) {
//       env->decay_steps = 0;
//     } else {
//       env->db_attenuate = 960.0f;
//     }
//   }
//   if (env->db_attenuate < 0.0) {
//     env->db_attenuate = 0.0f;
//   }
//   return env->db_attenuate;
// }
// void adsrRelease(adsr_t *env) {
//   env->decay_steps = 0;
//   env->att_steps = 0;
//   env->release_steps = env->db_attenuate / 1000.0f * (env->release_steps);
// }

int main(int argc, char **argv) {
  char *readff = argc > 1 ? argv[1] : "file.sf2";
  char *filename = argc > 2 ? argv[2] : "file.html";
  readsf(readff);
  PresetZones *pz = findByPid(0, 0);

  zone_t *zone = *ffilterForZone(findByPid(0, 0), 44, 120);
  shdrcast *sh = (shdrcast *)(shdrs + zone->SampleId);
  int tablesize = sh->endloop - sh->startloop;

  printf("%hd", zone->FilterFc);
  voice_t *v = (voice_t *)malloc(sizeof(voice_t));
  v->frac = 0.0f;
  float rootkey =
      (zone->OverrideRootKey > -1 ? zone->OverrideRootKey : sh->originalPitch) *
      100.0f;
  float pitch = rootkey + zone->CoarseTune * 100 + zone->FineTune;
  float pitchshift = 4400 - pitch;
  printf("\n%f \n", pitchshift);
  v->phase = sh->startloop;
  v->phaseShift = (int32_t)(0xfffffffe) / sh->sampleRate *
                  powf(2, pitchshift / 1200.0f) * 440.0f;

  v->pos = sh->start;
  v->ratio = (float)sh->sampleRate / (powf(2, (pitch - 6900) / 1200) * 440.0f) /
             4096.0f;
  float wavetables[48000];
  for (int i = 0; i < 48000; i++) {
    float fm1 = *(sdta + v->pos - 1);
    float f1 = *(sdta + v->pos);
    float f2 = *(sdta + v->pos + 1);
    float f3 = *(sdta + v->pos + 2);
    wavetables[i] = hermite4(v->frac, fm1, f1, f2, f3);
    v->frac += v->ratio;
    if (v->frac >= 1.0f) {
      v->frac--;
      v->pos++;
    }
  }
  FILE *Fd = fopen(sh->name, "wb");
  fwrite(wavetables, 4, 48000, Fd);
  wavetable_oscillator_data *osc = init_oscillators();
  float *wavetable_ref = wavetables;

  osc->wave000 = wavetable_ref;
  osc->wave001 = wavetable_ref += 4 * WAVETABLE_SIZE;
  osc->fadeDim1 = 0.001f;
  osc->fadeDim1Increment = 1 / 4.0f / WAVETABLE_SIZE;
  set_midi(0, 55);
  // FILE *output = formatpcm("mp3", "note.mp3");
  FILE *output = ffp(1, 48000);
  for (int i = 0; i < 48000; i++) {
    wavetable_1dimensional_oscillator(osc);
    fwrite(osc->output_ptr, 4, SAMPLE_BLOCKSIZE, output);
    if (osc->fadeDim1 >= 1.0f) {
      osc->wave000 = wavetable_ref += 4 * WAVETABLE_SIZE;
      osc->fadeDim1Increment *= -1;
    } else if (osc->fadeDim1 <= 0.0f) {
      osc->wave001 = wavetable_ref += 4 * WAVETABLE_SIZE;
      osc->fadeDim1Increment *= -1;
    }
  }
}