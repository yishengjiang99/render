#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fft-samples.h"
#include "glue.c"
#include "libs/wavetable_oscillator.c"
/**
 *  fast! furious? and thread-safe!!!
 **/

extern void fetch_await_buffer(char* url, char* buf, int len, void* opaque,
                               void (*cb)(void* opaque, void* data));
typedef struct {
  int i, j;
  float ifl, jfl;
} table_index;
static wavetable_set wavetable_sets[NUM_OSCILLATORS];
static table_index active_tables[NUM_OSCILLATORS];

typedef struct {
  char cmd, byte2, byte3, malign;
} midi_chan_msg;

static char midi_msg_queue[256];
volatile char rdx = 0;
volatile char wdx = 0;
void midi_put_c(char msg) { midi_msg_queue[wdx++] = msg; }

midi_chan_msg* midi_read_msg() {
  if (rdx + 4 < wdx) return NULL;
  midi_chan_msg* msg = (midi_chan_msg*)midi_msg_queue + rdx;
  rdx += 4;
  return msg;
}

void fetch_got(void* opaque, void* data) {
  int idx = *((int*)(data));
  wavetable_sets[idx] = *(wavetable_set*)data;
}

void setProgram(int channelId, int pid, int bankid) {
  char fname[127];
  sprintf(fname, "pages/%d_%d.dat", pid, bankid);
  fetch_await_buffer(fname, (char*)wavetable_sets + channelId,
                     sizeof(wavetable_set), (void*)&channelId, &fetch_got);
}

#define calcIdx(key, vel)                      \
  {                                            \
    ifl = (key - 20) * (nkeys * 1.0f) / 80.0f; \
    jfl = (float)vel / 120.0f * nvels;         \
    i = (int)ifl;                              \
    j = (int)jfl;                              \
  }

void proc_note_on(int channelId, int key, int vel) {
  int i, j;
  float ifl, jfl;
  calcIdx(key, vel);
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];

  osc.wave000 = tset.init_att[i][j];
  osc.wave001 = tset.eg_peak[i][j];
  osc.fadeDim1 = 0.0f;
  osc.fadeDim1Increment = tset.envelope[i][j][ATT];

  osc.wave010 = tset.init_att[i + 1][j];
  osc.wave011 = tset.eg_peak[i + 1][j];
  osc.fadeDim2 = ifl - i;
  osc.fadeDim2Increment = 0.f;

  osc.wave100 = tset.init_att[i + 0][j + 1];
  osc.wave101 = tset.eg_peak[i + 0][j + 1];
  osc.wave110 = tset.init_att[i + 1][j + 1];
  osc.wave111 = tset.eg_peak[i + 1][j + 1];
  osc.fadeDim3 = jfl - j;
  osc.fadeDim3Increment = 0.f;
  active_tables[channelId] = (table_index){i, j, ifl, jfl};
}

void proc_note_release(int channelId, int key, int vel) {
  table_index indices = active_tables[channelId];
  int i = indices.i;
  int j = indices.j;
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];
  osc.wave000 = silence;
  osc.fadeDim1Increment = -1.0f / (float)tset.envelope[i][j][REL];
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
  tset.envelope[i][j][DEC] * (960.f / (float)tset.envelope[i][j][SUS]);

void proc_att_finish(int channelId) {
  table_index indices = active_tables[channelId];
  int i = indices.i;
  int j = indices.j;
  wavetable_oscillator_data osc = oscillator[channelId];
  wavetable_set tset = wavetable_sets[channelId];
  osc.wave000 = silence;
  osc.wave010 = silence;
  osc.wave110 = silence;
  osc.fadeDim1Increment = -1.0f / decay_steps(i, j);
}
void check_msg() {
  midi_chan_msg* m;
  while ((m = midi_read_msg()) != NULL) {
    int cid = m->cmd & 0x0f;
    switch (m->cmd & 0x80) {
      case PROGRAM_CHANGE:
        setProgram(cid, m->byte2, cid == 9 ? 128 : 0);
        break;
      case NOTE_ON:
        proc_note_on(cid, m->byte2 & 0x7f, m->byte3 & 0x7f);
        break;
      case NOTE_OFF:
        proc_note_release(cid, m->byte2 & 0x7f, m->byte3 & 0x7f);
        break;
      default:
        break;
    }
  }
}
void spin_oscillator_event_loop(float* ob, int ob_len) {
  midi_chan_msg* m;
  for (int i = 0; i < ob_len; i += SAMPLE_BLOCKSIZE) {
    check_msg();
    for (int channelId = 0; channelId < NUM_OSCILLATORS; channelId++) {
      wavetable_oscillator_data _osc = oscillator[channelId];
      if (_osc.fadeDim1Increment != 0.0f) {
        wavetable_oscillator_data _osc = oscillator[channelId];
        wavetable_set tset = wavetable_sets[channelId];
        wavetable_3dimensional_oscillator(&_osc);
        if (_osc.fadeDim1 >= 0.999f) {
          proc_att_finish(channelId);
        }
        if (_osc.fadeDim1 < 0.0f) {
          _osc.fadeDim1Increment = 0.0f;
        }
        for (int k = 0; k < SAMPLE_BLOCKSIZE; k++) {
          *(ob + i + k) += _osc.output_ptr[k];
        }
      }
    }
  }
}
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