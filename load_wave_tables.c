
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "call_ffp.c"
#include "fft-samples.h"
#include "libs/wavetable_oscillator.c"

/**
 *  fast! furious? and thread-safe!!!
 **/

// extern void fetch_await_buffer(char* url, char* buf, int len);

typedef struct {
  int i, j;
  float ifl, jfl;
} table_index;
static wavetable_set wavetable_sets[NUM_OSCILLATORS];
static table_index active_tables[NUM_OSCILLATORS];

void setProgram(int channelId, int pid, int bankid) {
  char fname[127];
  sprintf(fname, "pages/%d_%d.dat", pid, bankid);
  FILE *fd = fopen(fname, "r");
  if (!fd) {
    perror("\n no fd");
  }
  fread(wavetable_sets + channelId, sizeof(wavetable_set), 1, fd);
  printf("%2x", wavetable_sets[channelId].envelope[0][0][ATT]);
  fclose(fd);
}
#define c2s(ct) powf(2.0f, (float)ct / 1200.0f)
void proc_note_on(int channelId, int key, int vel) {
  set_midi(channelId, key);
  int i, j;
  float ifl, jfl;
  ifl = (key - 20) * (nkeys * 1.0f) / 80.0f;
  jfl = (float)vel / 120.0f * nvels;
  i = (int)ifl;
  j = (int)jfl;
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];
  set_midi(channelId, key);
  printf("u att length %hdw", tset.envelope[i][j][ATT]);
  osc.wave000 = tset.init_att[i][j];
  osc.wave001 = tset.eg_peak[i][j];
  osc.fadeDim1 = 0.0f;
  short attcent = tset.envelope[i][j][ATT];
  osc.fadeDim1Increment = 1.0f / (SAMPLE_RATE * c2s(attcent));

  osc.wave010 = tset.init_att[i + 1][j];
  osc.wave011 = tset.eg_peak[i + 1][j];
  osc.fadeDim2 = ifl - i;
  printf("\n%f", osc.fadeDim1Increment);
  osc.fadeDim2Increment = 0.f;

  osc.wave100 = tset.init_att[i + 0][j + 1];
  osc.wave101 = tset.eg_peak[i + 0][j + 1];
  osc.wave110 = tset.init_att[i + 1][j + 1];
  osc.wave111 = tset.eg_peak[i + 1][j + 1];
  osc.fadeDim3 = jfl - j;
  osc.fadeDim3Increment = 0.f;
  active_tables[channelId] = (table_index){i, j, ifl, jfl};
  printf("\n%f -- noteon, osc %f, fd1", osc.fadeDim1Increment,
         osc.phaseIncrement / SAMPLE_RATE);
}

void proc_note_release(int channelId, int key, int vel) {
  table_index indices = active_tables[channelId];
  int i = indices.i;
  int j = indices.j;
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];
  osc.wave000 = silence;
  osc.wave010 = silence;
  osc.wave110 = silence;
  osc.fadeDim1Increment =
      -1.0 * powf(2.0f, tset.envelope[i][j][REL] / 1200.0f) / SAMPLE_RATE;
  // still decaying toward same wavetable (silence).
}

/**
 * the pro-rated decay_steps(in terms of #samples).
 * decay is given as # of sr it takes to go from 0dbfs to
 * a sustain db level (in units of 0.1db)
 *
 * this linearly extends that slope to -96db (or 960 'cent'-ible)
 */
#define decay_steps(i, j) \
  tset.envelope[i][j][DEC] * (960.f / (float)tset.envelope[i][j][SUS])

void proc_att_finish(int channelId) {
  table_index indices = active_tables[channelId];
  int i = indices.i;
  int j = indices.j;
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];
  osc.wave000 = tset.decay[i][j];
  osc.wave010 = tset.decay[i][j + 1];
  osc.wave110 = tset.decay[i + 1][j + 1];

  osc.fadeDim1Increment =
      1.0f / SAMPLE_RATE / powf(2.0f, decay_steps(i, j) / 1200.0f);
}

typedef enum {
  NOTE_OFF = 0x80,
  NOTE_ON = 0x90,
  POLY_KEY_PRESSURE = 0xA0,
  CONTROL_CHANGE = 0xB0,
  PROGRAM_CHANGE = 0xC0,
  CHANNEL_PRESSURE = 0xD0,
  PITCH_BEND_CHANGE = 0xE0
} midi_status;

typedef enum {
  string_ens = 49,
  oboe = 69,
  eng_horn,
  bassoon,
  clarinet,
  Trumpet = 57,
  viola = 42,
  cello,
  idc,
  Tremolo,
  pizzicato,
  idc2,
  timpani
} instrument;

typedef struct {
  char cmd, byte2, byte3, malign;
} midi_chan_msg;

static char midi_msg_queue[256];
volatile char rdx = 0;
volatile char wdx = 0;
void load_from_mem(char *buf, int len) {}
void midi_put_c(char msg) { midi_msg_queue[wdx++] = msg; }

midi_chan_msg *midi_read_msg() {
  if (rdx + 4 < wdx) return 0;
  midi_chan_msg *msg = (midi_chan_msg *)midi_msg_queue + rdx;
  rdx += 4;
  return msg;
}

void spin_oscillator_event_loop(float *ob, int ob_len) {
  for (int i = 0; i < ob_len; i += SAMPLE_BLOCKSIZE) {
    // midi_read_msg();
    for (int channelId = 0; channelId < NUM_OSCILLATORS; channelId++) {
      wavetable_oscillator_data _osc = oscillator[channelId];
      // printf("\n %d", _osc.phaseIncrement);

      if (_osc.phaseIncrement > 0.0f) {
        // printf("\n %d", _osc.phaseIncrement);

        wavetable_oscillator_data _osc = oscillator[channelId];
        bzero(_osc.output_ptr, sizeof(float) * SAMPLE_BLOCKSIZE);
        wavetable_3dimensional_oscillator(&_osc);
        if (_osc.fadeDim1 >= 0.999f) {
          proc_att_finish(channelId);
        }
        oscillator[0].fadeDim1 += 0.001f;

        for (int k = 0; k < SAMPLE_BLOCKSIZE; k += 2) {
          *(ob + i + k) += _osc.output_ptr[k];
          //  *(ob + i + k + 1) += _osc.output_ptr[k];
        }
      }
    }
  }
}
#include <string.h>
int main() {
  float ob[128];
  init_oscillators();
  setProgram(9, 0, 128);
  FILE *o = ffp(1, 48000);

  proc_note_on(9, 66, 120);
  for (int i = 0; i < 48000 * .5; i += 128) {
    bzero((oscillator + 9)->output_ptr, sizeof(float) * SAMPLE_BLOCKSIZE);
    wavetable_2dimensional_oscillator(oscillator + 9);
    fwrite((oscillator + 9)->output_ptr, sizeof(float), SAMPLE_BLOCKSIZE, o);
  }
  // }
  // FILE *o = ffp(1, 48000);
  // float vm = 1.0f;
  // for (int a = clarinet; a >= 0; a--) {
  //   bzero(wavetable_sets, sizeof(wavetable_sets));
  //   setProgram(a / 8, a, 0);
  //   wavetable_oscillator_data *_osc = &(oscillator[a / 8]);

  //   for (int midi = 55; midi < 70; midi += vm) {
  //     if (midi % 12 == 0)
  //       vm = 3;
  //     else
  //       vm = 4;
  //     _osc->frequencyIncrement = -90;

  //     proc_note_on(a / 8, midi, midi % 4 * 10);
  //     for (int i = 0; i < 48000 * .5; i += 128) {
  //       bzero(_osc->output_ptr, sizeof(float) * SAMPLE_BLOCKSIZE);
  //       wavetable_3dimensional_oscillator(_osc);
  //       // oscillator[0].fadeDim1 += oscillator[0].fadeDim1Increment;
  //       _osc->frequencyIncrement *= -.80;
  //       fwrite(oscillator->output_ptr, sizeof(float), SAMPLE_BLOCKSIZE, o);
  //     }
  //     proc_att_finish(a / 8);

  //     proc_note_release(a / 8, midi, 122);
  //     for (int i = 0; i < 48000 * .2; i += 128) {
  //       bzero(_osc->output_ptr, sizeof(float) * SAMPLE_BLOCKSIZE);

  //       wavetable_3dimensional_oscillator(_osc);
  //       fwrite(_osc->output_ptr, sizeof(float), SAMPLE_BLOCKSIZE, o);
  //     }
  //   }
  // }
}