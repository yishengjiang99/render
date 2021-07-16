#ifndef FFT_SAMPLES_H
#define FFT_SAMPLES_H

#define output_sampleRate 48000
#define dspbuffersize 128

#define FFTBINS 4096

#define nkeys 6
#define nvels 5
typedef unsigned int uint32_t;
typedef struct {
  short envelope[nkeys][nvels][4];
  float init_att[nkeys][nvels][FFTBINS];
  float eg_peak[nkeys][nvels][FFTBINS];
  float loop[nkeys][nvels][FFTBINS];
  float decay[nkeys][nvels][FFTBINS];

} wavetable_set;

typedef enum { ATT, DEC, REL, SUS } ADRS;

#endif