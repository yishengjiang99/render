#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "ctx.c"
#include <stdlib.h>
#include <time.h>
void *rend_fn(void *arg)
{
	ctx_t *ctx = (ctx_t *)arg;
	struct timespec start, stop;
	long elapsed;
	for (;;)
	{
		clock_gettime(CLOCK_REALTIME, &start);
		render(ctx);
		clock_gettime(CLOCK_REALTIME, &stop);
		elapsed = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
		usleep(1e3 - elapsed);
	}
}
void *receive_fn(void *arg)
{
	ctx_t *ctx = (ctx_t *)arg;

	printf("thread: serving fd %ld\n", ctx->outputFD);
	char buf[1024];

	for (;;)
	{

		int bytes_received = recv(ctx->outputFD, buf, sizeof(buf), 0);
		switch (buf[0] & 0x7f)
		{
		case 144:
			noteOn(ctx, buf[0] & 0x80, buf[1] & 0x7f, buf[2] & 0x7f);
			break;
		case 128:
			noteOff(ctx, buf[0] & 0x80, buf[1] & 0x7f);
			break;
		case 192:
			ctx->channels[buf[0] & 0x80].program_number = buf[1] & 0x7f;
			if ((buf[0] & 0x80) == 9)
			{
				ctx->channels[buf[0] & 0x80].program_number += 0x80;
			}
			break;
		default:
			break;
		}
		usleep(10e6);
	}

stop_serving:
	close(ctx->outputFD);
	printf("thread: finished serving %ld\n", ctx->outputFD);
	return NULL;
}

int main(void)
{
	FILE *dl = fopen("file.sf2", "rb");
	readsf(dl);
	fclose(dl);
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	listen(listen_fd, 100);
	struct sockaddr_in addr;
	socklen_t addr_bytes = sizeof(addr);
	getsockname(listen_fd, (struct sockaddr *)&addr, &addr_bytes);
	printf("Listening on port %d\n", ntohs(addr.sin_port));

	for (;;)
	{
		intptr_t conn_fd = accept(listen_fd, NULL, NULL);
		ctx_t *ctx = init_ctx();
		ctx->outputFD = conn_fd;

		pthread_t read_thread, write_thread;
		pthread_create(&read_thread, NULL, receive_fn, (void *)ctx);
		pthread_create(&read_thread, NULL, rend_fn, (void *)ctx);

		printf("main: created thread to handle connection %ld\n", conn_fd);
	}
	return 0;
}