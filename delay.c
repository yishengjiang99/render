#include <string.h>
typedef unsigned int uint32_t;
typedef struct _delay
{
	uint32_t buffer_size;
	uint32_t delay_time_in_samples;
	uint32_t input_size;
	uint32_t write_offset;
	float *delaybuffer;
	float *output;
} delay_data;

delay_data *newDelay(uint32_t delaySamples)
{
	delay_data *dl = (delay_data *)malloc(sizeof(delay_data));
	dl->buffer_size = delaySamples;
	dl->delay_time_in_samples = delaySamples;
	dl->input_size = 128;
	dl->write_offset = 0;
	dl->delaybuffer = (float *)malloc(sizeof(float) * delaySamples);
	return dl;
}

float *inputPtr(delay_data *dl)
{
	return dl->delaybuffer + dl->write_offset;
}
void delay_snds(delay_data *delay)
{
	//	memcpy(delay->delaybuffer + delay->write_offset, delay->inputwave, delay->input_size);

	//* in forcing the write ptr to neve wraparound in middle of a block
	// we makesd ure read ptr doesn't do same as well.
	int readPtr = delay->write_offset - delay->delay_time_in_samples;

	delay->output = delay->delaybuffer + readPtr;
	delay->write_offset += delay->input_size;
	if (delay->write_offset + delay->input_size > delay->buffer_size)
	{
		delay->write_offset = 0;
	}
}
