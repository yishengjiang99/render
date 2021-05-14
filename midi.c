#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include "tml.h"
#include "ctx.c"
#include "call_ffp.c"

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define MSEC 1000L

void *cb(void *args)
{
	struct timespec start, stop;
	long elapsed;
	ctx_t *ctx = (ctx_t *)args;
	while (1)
	{

		clock_gettime(1, &start);
		render(ctx);
		usleep(3 * MSEC);
	}

	return NULL;
}
int main()
{
	ctx_t *ctx = init_ctx();
	readsf(fopen("file.sf2", "rb"));
	//ctx->outputFD = ffplay("wav", "output.wav"); //2, 48000);
	ctx->outputFD = ffp(2, 48000);
	ctx->outputFD = popen("ffmpeg -y -f f32le -i pipe:0 -ac 2 -f flac 5558.flac", "w");
	tml_message *m = tml_load_filename("song.mid");
	int msec = 0;
	//	ctx->outputFD = fopen("/dev/stdout", "w");
	pthread_t t;
	pthread_create(&t, NULL, &cb, (void *)ctx);
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
					ctx->channels[m->channel].midi_gain_cb = midiCBlut(m->control_value);
					break;
				case TML_EXPRESSION_MSB:
					ctx->channels[m->channel].midi_gain_cb = midiCBlut(m->control_value); //m->control_value;
					break;
				}
				break;
			}
			case TML_ALL_NOTES_OFF:
			case TML_ALL_SOUND_OFF:
				ctx->voices = 0;
				ctx->fadeouts = 0;
				break;
			case TML_PROGRAM_CHANGE:
				if (m->program == 48)
				{
					m->program = 48 + m->channel + 1;
				}
				ctx->channels[m->channel].program_number = m->program;

				break;
			case TML_NOTE_ON:
				noteOn(ctx, (int)m->channel, (int)m->key, (int)m->velocity, m->time);
				break;
			case TML_NOTE_OFF:
				noteOff(ctx, (int)m->channel, (int)m->key);
				break;
			default:
				break;
			}
			m = m->next;
		}
		usleep(MSEC * 3);
	}
}
// //pthread_join(audiothread, NULL);
// }