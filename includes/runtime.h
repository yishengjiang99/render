#ifndef RUNTIME_H
#define RUNTIME_H
#include <stdio.h>

#include "biquad.h"
#include "sf2.h"
#ifndef output_sampleRate
#define output_sampleRate 44100

#endif

#ifndef dspbuffersize
#define dspbuffersize 128
#endif

typedef struct {
  uint32_t delay_steps, att_steps, hold_steps, decay_steps, release_steps;
  unsigned short sustain;
  float db_attenuate;
  float att_rate, decay_rate, release_rate;
} adsr_t;
typedef enum { voice_rising, voice_decay, voice_release, voice_gc } voice_state;

typedef struct _voice {
  voice_state done;
  zone_t *z;
  shdrcast *sample;
  unsigned int start, end, startloop, endloop;
  uint32_t pos;
  float frac;
  float ratio;
  adsr_t *ampvol, *moddvol;
  int midi;
  int velocity;
  int chid;
  float panLeft, panRight;
  short attenuate;
  biquad *lpf;
  struct _voice *next;
} voice;

typedef struct {
  int program_number;
  float midi_volume;
  float midi_pan;
  PresetZones *pzset;
  voice *voices;
} channel_t;

typedef struct _ctx {
  int sampleRate;
  uint16_t currentFrame;
  int samples_per_frame;
  int refcnt;
  float mastVol;
  channel_t channels[16];
  float *outputbuffer;
  FILE *outputFD;
} ctx_t;

void applyZone(zone_t *z, int midi, int vel, int cid);

static ctx_t *g_ctx;
int get_sf(int channelNumer, int key, int vel);

adsr_t *newEnvelope(short centDelay, short centAtt, short centHold,
                    short centRelease, short centDecay, short sustain,
                    int sampleRate);
float envShift(adsr_t *env);
void adsrRelease(adsr_t *env);

voice *newVoice(zone_t *z, int midi, int vel, int cid);
ctx_t *init_ctx();
void setProgram(int channelNumber, int presetId);
void noteOn(int channelNumber, int midi, int vel, unsigned long when);
void noteOff(int i, int midi);
void render(ctx_t *ctx);

void render_fordr(ctx_t *ctx, float duration, void (*cb)(ctx_t *ctx));
void loop(voice *v, float *output, channel_t ch);
void loopreal(voice *v, double *output);
float p2over1200(float x);
float centdblut(float x);
float midiCBlut(int midi);
float midiCBlut(int midi);
float panLeftLUT(short Pan);
float centtone2freq(unsigned short ct);
float centdblut(float x);
#endif