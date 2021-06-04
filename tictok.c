#ifndef TICTOK_C
#define TICTOK_C

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L;
static struct timespec start, stop;
static double accum;
#define TIC() \
	accum = 0;  \
	clock_gettime(CLOCK_MONOTONIC, &start);

#define TOK()                                      \
	if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1) \
	{                                                \
		perror("clock gettime");                       \
		exit(EXIT_FAILURE);                            \
	}

double tiktoktime()
{
	accum = (stop.tv_sec - start.tv_sec) * BILLION;
	accum += (stop.tv_nsec - start.tv_nsec);
	return accum;
}

#endif