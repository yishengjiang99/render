#include "runtime.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#include "LFO.h"
#include "libs/biquad.h"
#include "luts.c"
#include "sf2.c"

#define minf(a, b) a < b ? a : b;
int get_sf(int channelNumer, int key, int vel) {
  channel_t ch = g_ctx->channels[channelNumer];
  zone_t *zones = ch.pzset.zones;
  int found = 0;
  for (int i = 0; i < ch.pzset.npresets - 1; i++, zones++) {
    if (zones == NULL) break;
    if (vel > -1 && (zones->VelRange.lo > vel || zones->VelRange.hi < vel))
      continue;
    if (key > -1 && (zones->KeyRange.lo > key || zones->KeyRange.hi < key))
      continue;
    newVoice(zones, key, vel, channelNumer);
    found++;
  }
  if (!found && vel > -1) return get_sf(channelNumer, key, -1);
  if (!found && key > -1) return get_sf(channelNumer, -1, vel);

  return found;
}

adsr_t *newEnvelope(short centDelay, short centAtt, short centHold,
                    short centRelease, short centDecay, short sustain,
                    int sampleRate) {
  if (centAtt <= -12000) centAtt = -11000;
  adsr_t *env = (adsr_t *)malloc(sizeof(adsr_t));
  env->delay_steps = powf(2.0f, (float)centDelay / 1200.0f) * sampleRate;
  env->hold_steps = powf(2.0f, (float)centHold / 1200.0f) * sampleRate;

  env->att_steps = powf(2.0f, (float)centAtt / 1200.0f) * sampleRate;
  env->decay_steps = powf(2.0f, (float)centDecay / 1200.0f) * sampleRate;
  env->release_steps = powf(2.0f, (float)centRelease / 1200.0f) * sampleRate;
  sustain = 500;
  env->att_rate = -960.0f / env->att_steps;
  env->decay_rate = (960.0f * sustain / 1000.f) / (float)env->decay_steps;
  env->release_rate = (float)960.0f / ((float)env->release_steps);
  env->db_attenuate = 959.0f;

  return env;
}
float envShift(adsr_t *env) {
  // printf("%d %d %f %d  %d %d %f\n", env->delay_steps, env->att_steps,
  //        env->att_rate, env->hold_steps, env->decay_steps,
  //        env->release_steps, env->db_attenuate);
  if (env->delay_steps > 0) {
    env->delay_steps--;
  }

  else if (env->att_steps > 0) {
    env->att_steps--;
    env->db_attenuate += env->att_rate;
  } else if (env->hold_steps > 0) {
    env->hold_steps--;
  } else if (env->decay_steps > 0) {
    env->decay_steps--;
    env->release_steps--;

    env->db_attenuate += env->decay_rate;
  } else if (env->release_steps > 0) {
    env->release_steps--;
    env->db_attenuate += env->release_rate;
  }
  if (env->db_attenuate > 961) {
    env->release_steps = 0;
    env->db_attenuate = 960.f;
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
void insertV(voice **start, voice *nv) {
  voice **tr = start;
  nv->done = voice_rising;
  while (*tr) tr = &((*tr)->next);
  nv->next = *tr;
  *tr = nv;
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
  float modLFOVal = LFO_roll(v->modlfo, g_ctx->samples_per_frame);
  float pitchModLFO = modLFOVal * v->z->ModLFO2Pitch;  // centz
  uint32_t loopLength = v->endloop - v->startloop;
  int krateCB = 0;
  krateCB += v->attenuate;
  krateCB -= modLFOVal * v->z->ModLFO2Vol;

  float g_left = v->panLeft;
  float g_right = v->panRight;
  float stride = v->ratio;
  int aRateCB = krateCB;
  for (int i = 0; i < g_ctx->samples_per_frame; i++) {
    float modEG = centdblut(envShift(v->moddvol) / 10);
    float pitchModEG = modEG * v->z->ModEnv2Pitch / 100.0f;
    aRateCB += envShift(v->ampvol);
    stride = stride * (12.0f + pitchModEG + pitchModLFO) / 12.0f;
    float fm1 = *(sdta + v->pos - 1);
    float f1 = *(sdta + v->pos);
    float f2 = *(sdta + v->pos + 1);
    float f3 = *(sdta + v->pos + 2);

    float o1 = *(output + 2 * i + 1);
    float o2 = *(output + 2 * i);
    float gain = hermite4(v->frac, fm1, f1, f2, f3);
    gain = gain * centdblut(aRateCB / 10);

    if (v->lpf != NULL) {
      gain = BiQuad(gain, v->lpf);
    }
    *(output + 2 * i) += gain * v->panRight;
    *(output + 2 * i + 1) += gain * v->panLeft;

    v->frac += stride;
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
  v->z = z;
  v->chid = cid;
  insertV(&g_ctx->channels[cid].voices, v);
  v->start = sh->start +
             ((unsigned short)(z->StartAddrCoarseOfs & 0x7f) << 15) +
             (unsigned short)(z->StartAddrOfs & 0x7f);
  v->end = sh->end + ((unsigned short)(z->EndAddrCoarseOfs & 0x7f) << 15) +
           (unsigned short)(z->EndAddrOfs & 0x7f);
  v->endloop = sh->endloop +
               ((unsigned short)(z->EndLoopAddrCoarseOfs & 0x7f) << 15) +
               (unsigned short)(z->EndLoopAddrOfs & 0x7f);
  v->startloop = sh->startloop +
                 (unsigned short)(z->StartLoopAddrCoarseOfs & 0x7f << 15) +
                 (unsigned short)(z->StartLoopAddrOfs & 0x7f);
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
  float velratio = (127.0f / (float)vel) * (127.0f / (float)vel);
  float velgain =
      powf(10.0, (velratio * velratio - z->Attenuation / 10) * 0.05f);

  v->panLeft = panLeftLUT(z->Pan);
  v->panRight = (1 - v->panLeft);
  v->attenuate = velratio * velratio - z->Attenuation / 10;

  if (z->FilterFc < 14000) {
    v->lpf = BiQuad_new(LPF, z->FilterQ / 10.0f,
                        powf(2, (float)z->FilterFc / 1200.0f),
                        g_ctx->sampleRate, 1.0);
  }
  g_ctx->refcnt++;
  v->modlfo = (LFO *)malloc(sizeof(LFO));
  v->modlfo->delay = z->ModLFODelay;
  lfo_set_frequency(v->modlfo, pow(2, z->ModLFOFreq / 1200) * 8.176);
  v->vibrlfo = (LFO *)malloc(sizeof(LFO));
  v->vibrlfo->delay = z->VibLFODelay;
  lfo_set_frequency(v->vibrlfo, pow(2, z->VibLFOFreq / 1200) * 8.176);
  // insertV(&(g_ctx->channels[channelNumber].voices), v);
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
  g_ctx->channels[channelNumber].pzset =
      findByPid(presetId, channelNumber == 9 ? 128 : 0);
  g_ctx->channels[channelNumber].program_number = presetId;
}
void noteOn(int channelNumber, int midi, int vel, unsigned long when) {
  //	assert(g_ctx->channels[channelNumber].pzset.npresets > 0);
  // g_ctx->channels[channelNumber].voices = NULL;
  void noteOff(int channelNumber, int midi);

  get_sf(channelNumber, midi, vel);
}

void noteOff(int i, int midi) {
  channel_t ch = g_ctx->channels[i];
  voice **tr = &ch.voices;

  while (*tr) {
    if ((*tr)->midi == midi) {
      adsrRelease((*tr)->ampvol);
    }
    tr = &(*tr)->next;
  }
}

void render(ctx_t *ctx) {
  bzero(ctx->outputbuffer, sizeof(float) * ctx->samples_per_frame * 2);
  int vs = 0;
  for (int i = 0; i < 1; i++) {
    channel_t ch = g_ctx->channels[i];
    if (!ch.voices) continue;
    for (voice **tr = &ch.voices; *tr; tr = &(*tr)->next) {
      if ((*tr)->done) continue;
      if ((*tr)->ampvol->release_steps <= 0) {
        voice *donev = *tr;
        (*tr)->done = voice_gc;
        //       *tr = (*tr)->next;
        g_ctx->refcnt--;
        // free(donev);
      } else {
        loop(*tr, g_ctx->outputbuffer, ch);
      }
    }
  }

  ctx->currentFrame++;
  float maxv = 1.0f, redradio = 1.0f;
  float postGain = ctx->mastVol;
  for (int i = 0; i < ctx->samples_per_frame; i++) {
    ctx->outputbuffer[i] *= postGain;
    float absf = ctx->outputbuffer[i] > 0.f ? ctx->outputbuffer[i]
                                            : -1 * ctx->outputbuffer[i];

    if (absf >= maxv) {
      // fprintf(stderr, "clipping at %f\n", absf);
      maxv = absf;
    }
    ctx->outputbuffer[i] =
        ctx->outputbuffer[i] > 1.0 ? 1.0 : ctx->outputbuffer[i];
    ctx->outputbuffer[i] =
        ctx->outputbuffer[i] < -1.0 ? -1.0 : ctx->outputbuffer[i];
  }

  if (ctx->outputFD != NULL)
    fwrite(ctx->outputbuffer, ctx->samples_per_frame * 2, 4, ctx->outputFD);
}
void render_fordr(ctx_t *ctx, float duration, void (*cb)(ctx_t *ctx)) {
  while (duration > 0.0f) {
    render(ctx);
    if (cb) cb(ctx);
    duration -= 0.00266f;
  }
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
    return 0.5f + pan[Pan + 500];
  }
}
