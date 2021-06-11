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

int main(int argc, char **argv) {
  ctx_t *ctx = init_ctx();
  readsf(fopen("GeneralUserGS.sf2", "rb"));
  long elapsed;
  ctx->outputFD = ffp(2, ctx->sampleRate);

  tml_message *m = tml_load_filename("song.mid");
  int msec = 0;
  for (int i = 0; i < 1300010; i++) {
    render(ctx);

    usleep(2650 * 0.7);  // * MSEC);
    ctx_t *ctx = g_ctx;
    msec += 2650 * 0.9 / 1000;
    while (m && m->time < msec) {
      switch (m->type) {
        case TML_CONTROL_CHANGE: {
          switch (m->control) {
            case TML_VOLUME_MSB:
              ctx->channels[m->channel].midi_volume =
                  (float)m->control_value / 127.0f;
              break;
            default:

              break;
          }
        }

        case TML_ALL_NOTES_OFF:
        case TML_ALL_SOUND_OFF:

          break;
        case TML_PROGRAM_CHANGE:

          //		ctx->channels[m->channel].program_number = m->program;
          setProgram(m->channel, m->program);
          break;
        case TML_NOTE_ON:
          if (m->velocity == 0) {
            noteOff((int)m->channel, (int)m->key);
          } else {
            noteOn((int)m->channel, (int)m->key, (int)m->velocity, m->time);
            // printf("\n%d**\n", g_ctx->refcnt);
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
  }

  return 1;
}

//}
