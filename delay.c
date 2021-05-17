#include <stdlib.h>
#define delay_buffer_size 4096
#define delay_input_size 128
#define write_index_mask 0x0000FFFUL

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef struct _delay
{

	uint32_t buffer_size;
	uint32_t input_size;
	int32_t write_offset;
	float *delaybuffer;

} delay_data;

delay_data *newDelay()
{
	delay_data *dl = (delay_data *)malloc(sizeof(delay_data));
	dl->buffer_size = delay_buffer_size;
	dl->input_size = delay_input_size;
	dl->write_offset = 0;
	dl->delaybuffer = (float *)malloc(sizeof(float) * delay_buffer_size);
	return dl;
}

void advance_write_ptr(delay_data *dl)
{
	dl->write_offset = dl->write_offset + (dl->input_size << 1L);
}
float *write_ptr(delay_data *delay)
{
	return delay->delaybuffer + (delay->write_offset & write_index_mask);
}
float *read_ptr(delay_data *delay)
{
	return delay->delaybuffer + (((delay->write_offset - delay_buffer_size) << 1) & write_index_mask);
}