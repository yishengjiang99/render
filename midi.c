#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include "tml.h"
#include "sf2.c"
#include "runtime.c"

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define BILLION 1000000000L
#define MSEC 1000L

void *cb(void *args)
{
	ctx_t *ctx = (ctx_t *)args;
	struct timespec start, stop;
	long elapsed;
	ctx->outputFD = ffp(2, 48000);

	for (;;)
	{
		clock_gettime(0, &start);
		render(ctx);
		clock_gettime(0, &stop);
		elapsed = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);

		usleep(30000);
	}

	return NULL;
}
int main(int argc, char **argv)
{
	tml_message *m = tml_load_filename("song.mid");

	channel_t *ch;
	ctx_t *ctx = init_ctx();
	readsf(fopen("file.sf2", "rb"));

	tml_message *m = tml_load_filename("song.mid");
	int msec = 0;

	pthread_t t;
	//pthread_create(&t, NULL, &cb, (void *)ctx);
	while (m != NULL)
	{
		msec += 3;
		while (m && m->time < msec)
		{
			switch (m->type)
			{
			case TML_CONTROL_CHANGE:
			{
				switch (m->control)
				{

				case TML_VOLUME_MSB:
					ch = &(ctx->channels[m->channel]);
					ch->midi_volume = m->control_value;
					break;
				default:
					printf("cc %s %x %x\n", m->control, m->channel, m->control_value);

					break;
				}
			}

			case TML_ALL_NOTES_OFF:
			case TML_ALL_SOUND_OFF:

				break;
			case TML_PROGRAM_CHANGE:

				ctx->channels[m->channel].program_number = m->program;

				break;
			case TML_NOTE_ON:
				if (m->velocity == 0)
				{
					noteOff(ctx, (int)m->channel, (int)m->key);
				}
				else
				{
					noteOn(ctx, (int)m->channel, (int)m->key, (int)m->velocity, m->time);
				}

				break;
			case TML_NOTE_OFF:
				noteOff(ctx, (int)m->channel, (int)m->key);
				break;
			default:
				break;
			}
			m = m->next;
		}
		usleep(3000);
	}
}
//}
