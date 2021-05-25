#define output_sampleRate 48000
#define dspbuffersize 32
typedef struct
{
	int program_number;
	unsigned short midi_volume;
	unsigned short midi_pan;
} channel_t;
