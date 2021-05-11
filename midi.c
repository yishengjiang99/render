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

// void *cb(void *args)
// {
// 	struct timespec start, stop;
// 	long elapsed;
// 	ctx_t *ctx = (ctx_t *)args;

// 	clock_gettime(1, &start);
// 	// readsf(fopen("file.sf2", "rb"));
// 	tml_message *m = tml_load_filename("song.mid");

// 	int msecs = 0;

// 	return NULL;
// }
int main()
{

	readsf(fopen("file.sf2", "r"));
	printf("%s", phdrs[0].name);

	ctx_t *ctx = init_ctx(ffp(1, 48000));
	ctx->channels[0].program_number = phdrs[0].pid;
	noteOn(ctx, 0, 55, 66);
	render_fordr(ctx, 2.0f);
}
// }

// initLUTs();

// tml_message *m = tml_load_filename("song.mid");
// int msec = 0;
// while (m)
// {
// 	while (m && m->time < msec + 50)
// 	{
// 		switch (m->type)
// 		{
// 		case TML_PROGRAM_CHANGE:
// 			ctx->channels[m->channel].program_number = 3;
// 			break;
// 		case TML_NOTE_ON:
// 			printf("%d", m->key);
// 			noteOn(ctx, (int)m->channel, (int)m->key, (int)m->velocity);
// 			break;
// 		case TML_NOTE_OFF:
// 			printf("%d", m->key);
// 			noteOff(ctx, (int)m->channel, (int)m->key);
// 			break;
// 		}
// 		m = m->next;
// 	}
// 	usleep(3 * 1e6);
// 	msec += 300;
// 	m = m->next;
// 	printf("%d", msec);
// }
// //pthread_join(audiothread, NULL);
// }