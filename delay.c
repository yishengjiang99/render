#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint32_t;
typedef struct _delay
{
	uint32_t size;
	float *buffer;
	float *end;
	float *read;
	float *write;

} delay_t;

delay_t *newDelay(uint32_t size, uint32_t delay)
{
	delay_t *t = (delay_t *)malloc(sizeof(delay_t));
	t->size = size;
	t->buffer = (float *)malloc(size * sizeof(float));
	t->end = t->buffer + size;
	t->read = t->buffer;
	t->write = t->buffer + delay;
	return t;
}

void dl_write(delay_t *t, int n, float *fl)
{

	while (n-- > 0)
	{

		if (t->write == t->end)
		{
			t->write = t->buffer;
		}
		*t->write++ = *fl++;

		t->read++;
		if (t->read == t->end)
		{
			t->read = t->buffer;
		}
	}
}