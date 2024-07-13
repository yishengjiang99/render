#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "runtime/runtime.c"
#include "sf2/sf2.c"
#include "tml.h"
#define BILLION 1000000000L
#define MSEC 1000L
#include "biquad/biquad.c"
#include "call_ffp.c"
void *cb(void *args) {
  ctx_t *ctx = (ctx_t *)args;
<<<<<<< HEAD
  struct timespec start, stop;
  long elapsed;
  ctx->outputFD = fopen("hello.pcm", "wb");

  for (;;) {
    clock_gettime(0, &start);
    render(ctx);
    clock_gettime(0, &stop);
    elapsed = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);

    usleep(1e3 - elapsed);
=======
  long elapsed;
  ctx->outputFD = ffp(2, 48000);

  for (;;) {
    render_fordr(ctx, 0.0266, NULL);
    usleep(266);  // * MSEC);
>>>>>>> ccb63045f94e41b01371b10a1e1f4968357b9326
  }

  return NULL;
}
int main(int argc, char **argv) {
  tml_message *m = tml_load_filename("song.mid");

  channel_t *ch;
  ctx_t *ctx = init_ctx();
  readsf("file.sf2");

  int msec = 0;

  pthread_t t;
  // pthread_create(&t, NULL, &cb, (void *)ctx);
  while (m != NULL) {
<<<<<<< HEAD
    msec += 3;
=======
    msec += 5;
>>>>>>> ccb63045f94e41b01371b10a1e1f4968357b9326
    while (m && m->time < msec) {
      switch (m->type) {
        case TML_CONTROL_CHANGE: {
          switch (m->control) {
            case TML_VOLUME_MSB:
              ch = &(ctx->channels[m->channel]);
              ch->midi_volume = m->control_value;
              break;
            default:
              printf("cc %x %x %x\n", m->control, m->channel, m->control_value);

              break;
          }
        }

        case TML_ALL_NOTES_OFF:
        case TML_ALL_SOUND_OFF:

          break;
        case TML_PROGRAM_CHANGE:

<<<<<<< HEAD
          //				ctx->channels[m->channel].program_number =
          //m->program;
=======
>>>>>>> ccb63045f94e41b01371b10a1e1f4968357b9326
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
<<<<<<< HEAD
    usleep(3000);
=======
    usleep(5000);
>>>>>>> ccb63045f94e41b01371b10a1e1f4968357b9326
  }
  pthread_cancel(t);
  fclose(g_ctx->outputFD);
}
//}
