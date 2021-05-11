#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "ctx.c"
void *thread_func(void *arg)
{
	intptr_t conn_fd = (int)arg;
	printf("thread: serving fd %ld\n", conn_fd);
	char buf[1024];
	ctx_t *t = init_ctx();
	for (;;)
	{
		int bytes_received = recv(conn_fd, buf, sizeof(buf), 0);
		int a[4];
		//	sscanf(buf, "%d,%d,%d,%d\n", a[0], a[1], a[2], a[3]);
	}
stop_serving:
	close(conn_fd);
	printf("thread: finished serving %ld\n", conn_fd);
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
		pthread_t thread_id;
		int ret = pthread_create(&thread_id, NULL, thread_func, (void *)conn_fd);
		if (ret != 0)
		{
			printf("Error from pthread: %d\n", ret);
			exit(1);
		}
		printf("main: created thread to handle connection %ld\n", conn_fd);
	}
	return 0;
}