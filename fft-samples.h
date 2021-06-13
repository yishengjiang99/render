
#define output_sampleRate 4096
#define dspbuffersize 4096

#define FFTBINS 4096

#define nkeys 6
#define nvels 3
typedef unsigned int uint32_t;
typedef struct wavetable_set {
  uint32_t envelope[nkeys][nvels][4];
  float init_att[nkeys][nvels][FFTBINS];
  float eg_peak[nkeys][nvels][FFTBINS];
  float loop[nkeys][nvels][FFTBINS];
  float decay[nkeys][nvels][FFTBINS];
} wavetable_set;

typedef enum { ATT, DEC, REL, SUS } ADRS;
typedef enum {
  NOTE_OFF = 0x80,
  NOTE_ON = 0x90,
  POLY_KEY_PRESSURE = 0xA0,
  CONTROL_CHANGE = 0xB0,
  PROGRAM_CHANGE = 0xC0,
  CHANNEL_PRESSURE = 0xD0,
  PITCH_BEND_CHANGE = 0xE0
} midi_status;

typedef enum {
  string_ens = 49,
  oboe = 69,
  eng_horn,
  bassoon,
  clarinet,
  Trumpet = 57,
  viola = 42,
  cello,
  idc,
  Tremolo,
  pizzicato,
  idc2,
  timpani
} instrument;
