#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include "tml.h"
#include "sf2.c"
#include "runtime.c"
#include <unistd.h>
#include <pthread.h>
#include "call_ffp.c"

#define BILLION 1000000000L
#define MSEC 1000

void *cb(void *args)
{
	ctx_t *ctx = (ctx_t *)args;

	long elapsed;
	ctx->outputFD = ffp(2, 48000);

	for (;;)
	{
		render(ctx);

		usleep(2500); // * MSEC);
	}

	return NULL;
}
int main(int argc, char **argv)
{
	tml_message *m = tml_load_filename(argc > 1 ? argv[1] : "song.mid");
	channel_t *ch;
	init_ctx();
	readsf(fopen("file.sf2", "rb"));

	int msec = 0;

	pthread_t t;
	pthread_create(&t, NULL, &cb, (void *)g_ctx);
	ctx_t *ctx = g_ctx;
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
					noteOff((int)m->channel, (int)m->key);
				}
				else
				{
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
		usleep(3000);
	}
}
//}
