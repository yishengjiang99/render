#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include <pthread.h>
#include <unistd.h>

#include "call_ffp.c"
#include "runtime.c"
#include "sf2.c"
#include "tml.h"
#ifndef TICTOK_C
#define TICTOK_C

#include <time.h>

#define BILLION 1000000000L;
static struct timespec start, stop;
static double accum;
#define TIC() \
  accum = 0;  \
  clock_gettime(CLOCK_MONOTONIC, &start);

#define TOK()                                        \
  if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1) { \
    perror("clock gettime");                         \
    exit(EXIT_FAILURE);                              \
  }

double tiktoktime() {
  accum = (stop.tv_sec - start.tv_sec) * BILLION;
  accum += (stop.tv_nsec - start.tv_nsec);
  return accum;
}

#endif

void *cb(void *args) {
  ctx_t *ctx = (ctx_t *)args;
  long elapsed;
  ctx->outputFD = ffp(2, 48000);

  for (;;) {
    render_fordr(ctx, 0.0266, NULL);
    usleep(266);  // * MSEC);
  }

  return NULL;
}
int main(int argc, char **argv) {
  tml_message *m = tml_load_filename(argc > 1 ? argv[1] : "song.mid");
  channel_t *ch;
  init_ctx();
  readsf(fopen("GeneralUserGS.sf2", "rb"));

  int msec = 0;

  pthread_t t;
  g_ctx->mastVol = 1.0f;
  pthread_create(&t, NULL, &cb, (void *)g_ctx);
  ctx_t *ctx = g_ctx;
  while (m != NULL) {
    msec += 5;
    while (m && m->time < msec) {
      switch (m->type) {
        case TML_CONTROL_CHANGE: {
          switch (m->control) {
            case TML_VOLUME_MSB:
              ch = &(ctx->channels[m->channel]);
              ch->midi_volume = (float)m->control_value / 127.0f;
              break;
            default:

              break;
          }
        }

        case TML_ALL_NOTES_OFF:
        case TML_ALL_SOUND_OFF:

          break;
        case TML_PROGRAM_CHANGE:

          setProgram(m->channel, m->program);
          break;
        case TML_NOTE_ON:
          if (m->velocity == 0) {
            noteOff((int)m->channel, (int)m->key);
          } else {
            noteOn((int)m->channel, (int)m->key, (int)m->velocity, m->time);
          }

          break;
        case TML_NOTE_OFF:
          noteOff((int)m->channel, (int)m->key);
          break;
        default:
          break;
      }
      m = m->next;
    }
    usleep(5000);
  }
  pthread_cancel(t);
  fclose(g_ctx->outputFD);
}
//}
