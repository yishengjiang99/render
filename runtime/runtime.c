
#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#include "../index.h"
#include "luts.c"

#define minf(a, b) a < b ? a : b;
void loopreal(voice *v, double *output);
//(presetZones[i].zones[j].KeyRange & 0x7f00) >> 8)

adsr_t *newEnvelope(short centDelay, short centAtt, short centHold,
                    short centRelease, short centDecay, short sustain,
                    int sampleRate) {
  adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
  env->delay_steps = powf(2.0f, (float)centDelay / 1200.0f) * sampleRate;
  env->hold_steps = powf(2.0f, (float)centHold / 1200.0f) * sampleRate;

  env->att_steps = powf(2.0f, (float)centAtt / 1200.0f) * sampleRate;
  env->decay_steps = powf(2.0f, (float)centDecay / 1200.0f) * sampleRate;
  env->release_steps = powf(2.0f, (float)centRelease / 1200.0f) * sampleRate;
  env->att_rate = -960.0f / env->att_steps;
  env->decay_rate = ((float)1.0f * sustain) / (float)env->decay_steps;
  env->release_rate = (float)960.0f / ((float)env->release_steps);
  env->db_attenuate = 959.0f;

  return env;
}
float envShift(adsr_t *env) {
  if (env->delay_steps-- > 0) {
  }

  else if (env->att_steps > 0) {
    env->att_steps--;
    env->db_attenuate += env->att_rate;
  } else if (env->hold_steps-- > 0) {
  } else if (env->decay_steps > 0) {
    env->decay_steps--;
    env->release_steps--;

    env->db_attenuate += env->decay_rate;
  } else if (env->release_steps > 0) {
    env->release_steps--;
    env->db_attenuate += env->release_rate;
  }
  // printf("%f", env->db_attenuate);
  if (env->db_attenuate > 960) {
    if (env->att_steps <= 0) {
      env->decay_steps = 0;
    } else {
      env->db_attenuate = 960.0f;
    }
  }
  if (env->db_attenuate < 0.0) {
    env->db_attenuate = 0.0f;
  }
  return env->db_attenuate;
}
void adsrRelease(adsr_t *env) {
  env->decay_steps = 0;
  env->att_steps = 0;
  env->release_steps = env->db_attenuate / 1000.0f * (env->release_steps);
}
float hermite4(float frac_pos, float xm1, float x0, float x1, float x2) {
  const float c = (x1 - xm1) * 0.5f;
  const float v = x0 - x1;
  const float w = c + v;
  const float a = w + v + (x2 - x0) * 0.5f;
  const float b_neg = w + a;

  return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

float calcratio(zone_t *z, shdrcast *sh, int midi) {
  short rt = z->OverrideRootKey > -1 ? z->OverrideRootKey : sh->originalPitch;
  float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
  float octaveDivv = (float)midi * 100 - sampleTone;
  return powf(2.0f, octaveDivv / 1200.0f) * (float)sh->sampleRate /
         g_ctx->sampleRate;
}
#define trval (*tr)
#define trshift &((*tr)->next)
void loop(voice *v, float *output, channel_t ch) {
  loopreal(v, (double *)output);  // float *output);
}
void loopreal(voice *v, double *output) {
  uint32_t loopLength = v->endloop - v->startloop;

  float volume_gain = v->attenuate;  //* ch.midi_volume;
  float g_left = v->panLeft * volume_gain;
  float g_right = v->panRight * volume_gain;
  float modEG = envShift(v->moddvol) / -960.0f;
  for (int i = 0; i < g_ctx->samples_per_frame; i++) {
    modEG = envShift(v->moddvol) / -960.0f;

    float fm1 = *(sdta + v->pos - 1);
    float f1 = *(sdta + v->pos);
    float f2 = *(sdta + v->pos + 1);
    float f3 = *(sdta + v->pos + 2);

    float gain = hermite4(v->frac, fm1, f1, f2, f3);
    gain *= centdblut(envShift(v->ampvol));
    if (v->lpf != NULL) {
      gain = BiQuad(gain, v->lpf);
    }
    *(output + 2 * i) += (double)gain * v->panLeft;
    *(output + 2 * i + 1) += (double)gain * v->panRight;

    v->frac += v->ratio;  // p2over1200(modEG * v->z->ModEnv2Pitch);
    while (v->frac >= 1.0f) {
      v->frac--;
      v->pos++;
    }
    while (v->pos >= v->endloop) {
      if (v->z->SampleModes == 0) {
        v->ampvol->release_steps = 0;
        return;
      }
      v->pos = v->pos - loopLength + 1;
    }
  }
}

voice *newVoice(zone_t *z, int midi, int vel, int cid) {
  voice *v = (voice *)malloc(sizeof(voice));
  v->next = NULL;
  shdrcast *sh = (shdrcast *)(shdrs + z->SampleId);
  v->chid = cid;
  // insertV(&g_ctx->channels[cid].voices, v);
  v->start = sh->start + (z->StartAddrCoarseOfs << 15) + z->StartAddrOfs;
  v->end = sh->end + (z->EndAddrCoarseOfs << 15) + z->EndAddrOfs;
  v->endloop =
      sh->endloop + (z->EndLoopAddrCoarseOfs << 15) + z->EndLoopAddrOfs;
  v->startloop =
      sh->startloop + (z->StartLoopAddrCoarseOfs << 15) + z->StartLoopAddrOfs;

  v->ampvol = newEnvelope(z->VolEnvDelay, z->VolEnvAttack, z->VolEnvHold,
                          z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain,
                          g_ctx->sampleRate);

  v->moddvol = newEnvelope(z->ModEnvDelay, z->ModEnvAttack, z->ModEnvHold,
                           z->ModEnvRelease, z->ModEnvDecay, z->ModEnvSustain,
                           g_ctx->sampleRate);

  v->ratio = calcratio(z, sh, midi);
  v->pos = v->start;
  v->frac = 0.0f;
  v->z = z;
  v->midi = midi;
  v->velocity = vel;
  v->attenuate = powf(10.0, (z->Attenuation / 10) * 0.05f);
  float velratio = (127.0f / (float)vel) * (127.0f / (float)vel);

  v->panLeft = 0.5f + sinf((float)(Pan - 500.0f) / 1000.0f * M_2_PI);

  v->panRight = 0.5f + sinf((float)(Pan + 500.0f) / 1000.0f * M_2_PI);
  if (z->FilterFc * 2 < sh->sampleRate) {
    v->lpf = BiQuad_new(LPF, z->FilterQ / 10.0f,
                        p2over1200((float)z->FilterFc - 6900) * 440.0f,
                        sh->sampleRate, 1.0);
  }

  return v;
}

ctx_t *init_ctx() {
  g_ctx = (ctx_t *)malloc(sizeof(ctx_t));
  g_ctx->sampleRate = output_sampleRate;
  g_ctx->currentFrame = 0;
  g_ctx->samples_per_frame = dspbuffersize;
  for (int i = 0; i < 16; i++) {
    g_ctx->channels[i].program_number = 0;
    g_ctx->channels[i].midi_volume = 1.0f;
    g_ctx->channels[i].voices = NULL;  // newVoice(NULL, 0, 0, i);
  }
  g_ctx->refcnt = 0;
  g_ctx->mastVol = 1.0f;
  g_ctx->outputbuffer =
      (float *)malloc(sizeof(float) * g_ctx->samples_per_frame * 2);
  return g_ctx;
}
void setProgram(int channelNumber, int presetId) {
  channel_t *ch = &(g_ctx->channels[channelNumber]);
  ch->pzset = findByPid(presetId, channelNumber == 9 ? 128 : 0);
  ch->program_number = presetId;
}


float p2over1200(float x) {
  if (x < -12000) return 0;
  if (x < 0)
    return 1.f / p2over1200(-x);
  else if (x > 1200.0f) {
    return 2 * p2over1200(x - 1200.0f);
  } else {
    return p2over1200LUT[(unsigned short)(x)];
  }
}
float centdblut(float x) {
  if (x < 0) x = 0.0f;
  if (x > 960) x = 960.0f;

  return centdbLUT[(int)x];
}
float midiCBlut(int midi) {
  if (midi < 0) return -960.0f;
  if (midi > 960) return 0.0f;
  return midiCB[midi];
}
float centtone2freq(unsigned short ct) {
  return p2over1200(ct - 6900) * 440.0f;
}
float ct2relativePitch(short ct) { return p2over1200(ct); }

float panLeftLUT(short Pan) {
  if (Pan < -500) {
    return 1;
  }
  if (Pan > 500) {
    return 0.0f;
  } else {
    return 0.5f + sinf((float)(Pan - 500.0f) / 1000.0f * M_2_PI);
  }
}
