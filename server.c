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
		clock_gettime(0, &start);
		render(ctx);
		clock_gettime(0, &stop);
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5555
#define MAXMSG 512

int read_from_client(int filedes)
{
	char buffer[MAXMSG];
	int nbytes;

	nbytes = read(filedes, buffer, MAXMSG);
	if (nbytes < 0)
	{
		/* Read error. */
		perror("read");
		exit(EXIT_FAILURE);
	}
	else if (nbytes == 0)
		/* End-of-file. */
		return -1;
	else
	{
		/* Data read. */
		fprintf(stderr, "Server: got message: `%s'\n", buffer);
		return 0;
	}
}

int main(void)
{
	extern int make_socket(uint16_t port);
	int sock;
	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	size_t size;

	/* Create the socket and set it up to accept connections. */
	sock = make_socket(PORT);
	if (listen(sock, 1) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* Initialize the set of active sockets. */
	FD_ZERO(&active_fd_set);
	FD_SET(sock, &active_fd_set);

	while (1)
	{
		/* Block until input arrives on one or more active sockets. */
		read_fd_set = active_fd_set;
		if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}

		/* Service all the sockets with input pending. */
		for (i = 0; i < FD_SETSIZE; ++i)
			if (FD_ISSET(i, &read_fd_set))
			{
				if (i == sock)
				{
					/* Connection request on original socket. */
					int new;
					size = sizeof(clientname);
					new = accept(sock,
											 (struct sockaddr *)&clientname,
											 &size);
					if (new < 0)
					{
						perror("accept");
						exit(EXIT_FAILURE);
					}
					fprintf(stderr,
									"Server: connect from host %s, port %hd.\n",
									inet_ntoa(clientname.sin_addr),
									ntohs(clientname.sin_port));
					FD_SET(new, &active_fd_set);
				}
				else
				{
					/* Data arriving on an already-connected socket. */
					if (read_from_client(i) < 0)
					{
						close(i);
						FD_CLR(i, &active_fd_set);
					}
				}
			}
	}
}
