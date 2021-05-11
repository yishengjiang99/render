#include <stdio.h>
#include <stdlib.h>
#define TML_IMPLEMENTATION
#include "tml.h"
#include "ctx.c"
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

	clock_gettime(CLOCK_REALTIME, &start);

	while (1)
	{
		//clock_gettime(CLOCK_REALTIME, &start);
		//	printf("\n%ld %ld", start.tv_nsec % 1000000, elapsed);
		render(ctx);
		clock_gettime(CLOCK_REALTIME, &stop);
		elapsed = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / BILLION;
		usleep(3 * MSEC - elapsed);
		start = stop;
	}
}
int main()
{
	FILE *output = popen("ffplay -nodisp -f f32le -ac 2 -ar 48k -i pipe:0", "w");
	ctx_t *ctx = init_ctx(output);
	initLUTs();
	readsf(fopen("file.sf2", "rb"));
	tml_message *m = tml_load_filename("song.mid");
	pthread_t audiothread;
	pthread_create(&audiothread, NULL, &cb, ctx);

	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);
	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);
	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);
	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);
	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);
	noteOn(ctx, 0, 44, 32);
	sleep(1);
	noteOff(ctx, 0, 44); //, 32);

	noteOn(ctx, 0, 37, 44);
	sleep(1);
	noteOff(ctx, 0, 37); //, 32);
	usleep(1e6);

	pthread_kill(audiothread, 9);
	//	pthread_join(audiothread, NULL);
}