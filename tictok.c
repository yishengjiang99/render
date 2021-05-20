#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L;

int main(int argc, char **argv)
{
	struct timespec start, stop;
	double accum;

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
	{
		perror("clock gettime");
		exit(EXIT_FAILURE);
	}

	usleep(1000000);
	if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1)
	{
		perror("clock gettime");
		exit(EXIT_FAILURE);
	}

	accum = stop.tv_nsec - start.tv_nsec;
	printf("%lf\n", accum);
	return (EXIT_SUCCESS);
}