#include "wavetable_oscillator.h"
void wavetable_0dimensional_oscillator(
    wavetable_oscillator_data *this_oscillator) {
  float *out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;

  uint32_t phase = this_oscillator->phase;
  int32_t phaseIncrement = this_oscillator->phaseIncrement;
  int32_t frequencyIncrement = this_oscillator->frequencyIncrement;

  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  uint32_t mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;

  float *wave000 = this_oscillator->wave000;

  while (num_samples_remaining-- > 0) {
    unsigned int waveIndex0 =
        (unsigned int)(phase >> num_fractionalBits) & mask_waveIndex;
    unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
    float linearGain1 =
        scaler_fractionalBits * (float)(phase & mask_fractionalBits);
    float linearGain0 = 1.0f - linearGain1;

    float _wave000 =
        wave000[waveIndex0] * linearGain0 + wave000[waveIndex1] * linearGain1;

    phase += phaseIncrement;
    phaseIncrement += frequencyIncrement;

    *out++ = _wave000;
  }

  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}

void wavetable_1dimensional_oscillator(
    wavetable_oscillator_data *this_oscillator) {
  float *out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;

  uint32_t phase = this_oscillator->phase;
  int32_t phaseIncrement = this_oscillator->phaseIncrement;
  int32_t frequencyIncrement = this_oscillator->frequencyIncrement;

  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  uint32_t mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;

  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;

  float *wave000 = this_oscillator->wave000;
  float *wave001 = this_oscillator->wave001;
  while (num_samples_remaining-- > 0) {
    unsigned int waveIndex0 =
        (unsigned int)(phase >> num_fractionalBits) & mask_waveIndex;
    unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
    float linearGain1 =
        scaler_fractionalBits * (float)(phase & mask_fractionalBits);
    float linearGain0 = 1.0f - linearGain1;

    float _wave000 =
        wave000[waveIndex0] * linearGain0 + wave000[waveIndex1] * linearGain1;
    float _wave001 =
        wave001[waveIndex0] * linearGain0 + wave001[waveIndex1] * linearGain1;

    _wave000 += (_wave001 - _wave000) * fadeDim1;

    fadeDim1 += fadeDim1Increment;

    phase += phaseIncrement;
    phaseIncrement += frequencyIncrement;
    if (fadeDim1 < 0) {
      fadeDim1 = 0;
      fadeDim1Increment = 0;
    }

    *out++ = _wave000;
  }

  this_oscillator->fadeDim1 = fadeDim1;

  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}

void wavetable_2dimensional_oscillator(
    wavetable_oscillator_data *this_oscillator) {
  float *out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;

  uint32_t phase = this_oscillator->phase;
  int32_t phaseIncrement = this_oscillator->phaseIncrement;
  int32_t frequencyIncrement = this_oscillator->frequencyIncrement;

  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  uint32_t mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;

  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;
  float fadeDim2 = this_oscillator->fadeDim2;
  float fadeDim2Increment = this_oscillator->fadeDim2Increment;

  float *wave000 = this_oscillator->wave000;
  float *wave001 = this_oscillator->wave001;
  float *wave010 = this_oscillator->wave010;
  float *wave011 = this_oscillator->wave011;

  while (num_samples_remaining-- > 0) {
    unsigned int waveIndex0 =
        (unsigned int)(phase >> num_fractionalBits) & mask_waveIndex;
    unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
    float linearGain1 =
        scaler_fractionalBits * (float)(phase & mask_fractionalBits);
    float linearGain0 = 1.0f - linearGain1;

    float _wave000 =
        wave000[waveIndex0] * linearGain0 + wave000[waveIndex1] * linearGain1;
    float _wave001 =
        wave001[waveIndex0] * linearGain0 + wave001[waveIndex1] * linearGain1;
    float _wave010 =
        wave010[waveIndex0] * linearGain0 + wave010[waveIndex1] * linearGain1;
    float _wave011 =
        wave011[waveIndex0] * linearGain0 + wave011[waveIndex1] * linearGain1;

    _wave000 += (_wave010 - _wave000) * fadeDim2;
    _wave001 += (_wave011 - _wave001) * fadeDim2;

    _wave000 += (_wave001 - _wave000) * fadeDim1;

    fadeDim2 += fadeDim2Increment;
    fadeDim1 += fadeDim1Increment;

    phase += phaseIncrement;
    phaseIncrement += frequencyIncrement;

    *out++ = _wave000;
  }

  this_oscillator->fadeDim1 = fadeDim1;
  this_oscillator->fadeDim2 = fadeDim2;

  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}

void wavetable_3dimensional_oscillator(
    wavetable_oscillator_data *this_oscillator) {
  float *out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;

  uint32_t phase = this_oscillator->phase;
  int32_t phaseIncrement = this_oscillator->phaseIncrement;
  int32_t frequencyIncrement = this_oscillator->frequencyIncrement;

  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  uint32_t mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;

  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;
  float fadeDim2 = this_oscillator->fadeDim2;
  float fadeDim2Increment = this_oscillator->fadeDim2Increment;
  float fadeDim3 = this_oscillator->fadeDim3;
  float fadeDim3Increment = this_oscillator->fadeDim3Increment;

  float *wave000 = this_oscillator->wave000;
  float *wave001 = this_oscillator->wave001;
  float *wave010 = this_oscillator->wave010;
  float *wave011 = this_oscillator->wave011;
  float *wave100 = this_oscillator->wave100;
  float *wave101 = this_oscillator->wave101;
  float *wave110 = this_oscillator->wave110;
  float *wave111 = this_oscillator->wave111;

  while (num_samples_remaining-- > 0) {
    unsigned int waveIndex0 =
        (unsigned int)(phase >> num_fractionalBits) & mask_waveIndex;
    unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
    float linearGain1 =
        scaler_fractionalBits * (float)(phase & mask_fractionalBits);
    float linearGain0 = 1.0f - linearGain1;

    float _wave000 =
        wave000[waveIndex0] * linearGain0 + wave000[waveIndex1] * linearGain1;
    float _wave001 =
        wave001[waveIndex0] * linearGain0 + wave001[waveIndex1] * linearGain1;
    float _wave010 =
        wave010[waveIndex0] * linearGain0 + wave010[waveIndex1] * linearGain1;
    float _wave011 =
        wave011[waveIndex0] * linearGain0 + wave011[waveIndex1] * linearGain1;
    float _wave100 =
        wave100[waveIndex0] * linearGain0 + wave100[waveIndex1] * linearGain1;
    float _wave101 =
        wave101[waveIndex0] * linearGain0 + wave101[waveIndex1] * linearGain1;
    float _wave110 =
        wave110[waveIndex0] * linearGain0 + wave110[waveIndex1] * linearGain1;
    float _wave111 =
        wave111[waveIndex0] * linearGain0 + wave111[waveIndex1] * linearGain1;

    _wave000 += (_wave100 - _wave000) * fadeDim3;
    _wave001 += (_wave101 - _wave001) * fadeDim3;
    _wave010 += (_wave110 - _wave010) * fadeDim3;
    _wave011 += (_wave111 - _wave011) * fadeDim3;

    _wave000 += (_wave010 - _wave000) * fadeDim2;
    _wave001 += (_wave011 - _wave001) * fadeDim2;

    _wave000 += (_wave001 - _wave000) * fadeDim1;

    fadeDim3 += fadeDim3Increment;
    fadeDim2 += fadeDim2Increment;
    fadeDim1 += fadeDim1Increment;

    phase += phaseIncrement;
    phaseIncrement += frequencyIncrement;

    *out++ = _wave000;
  }

  this_oscillator->fadeDim1 = fadeDim1;
  this_oscillator->fadeDim2 = fadeDim2;
  this_oscillator->fadeDim3 = fadeDim3;

  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}

static wavetable_oscillator_data oscillator[NUM_OSCILLATORS];
static float sinewave[WAVETABLE_SIZE], squarewave[WAVETABLE_SIZE];
static float output_samples[NUM_OSCILLATORS][SAMPLE_BLOCKSIZE];
static float silence[WAVETABLE_SIZE] = {0.0f};
static float silence2[WAVETABLE_SIZE] = {0.0f};
static float sawtooth[WAVETABLE_SIZE] = {0.0f};

int wavetable_struct_size() { return sizeof(wavetable_oscillator_data); }

void set_midi(wavetable_oscillator_data *osc, int midiPitch) {
  float frequency = 440.0f;
  while (midiPitch-- > 69) frequency *= 1.05f;
  while (midiPitch++ < 69) frequency /= 1.05f;
  osc->phaseIncrement =
      (int32_t)(BIT32_NORMALIZATION * frequency / SAMPLE_RATE + 0.1f);
}
float fastsin(uint32_t phase) {
  const float frf3 = -1.0f / 6.0f;
  const float frf5 = 1.0f / 120.0f;
  const float frf7 = -1.0f / 5040.0f;
  const float frf9 = 1.0f / 362880.0f;
  const float f0pi5 = 1.570796327f;
  float x, x2, asin;
  uint32_t tmp = 0x3f800000 | (phase >> 7);
  if (phase & 0x40000000) tmp ^= 0x007fffff;
  x = (*((float *)&tmp) - 1.0f) * f0pi5;
  x2 = x * x;
  asin = ((((frf9 * x2 + frf7) * x2 + frf5) * x2 + frf3) * x2 + 1.0f) * x;
  return (phase & 0x80000000) ? -asin : asin;
}

wavetable_oscillator_data *init_oscillators() {
  //
  //	This sets up two wavetables for interpolation in one dimension.
  //
  // float sinewave[WAVETABLE_SIZE], squarewave[WAVETABLE_SIZE];

  for (int n = 0; n < WAVETABLE_SIZE; n++) {
    sinewave[n] = fastsin(n / WAVETABLE_SIZE *
                          BIT32_NORMALIZATION);  // (2.0 * PIF * ((float)n) /
                                                 // (float)WAVETABLE_SIZE);
    sawtooth[n] = (-2.0 + 2.0 * PIF * ((float)n) / (float)WAVETABLE_SIZE);
  }

  for (int n = 0; n < WAVETABLE_SIZE / 4; n++) {
    squarewave[n] = 0.5;
    squarewave[n + WAVETABLE_SIZE / 4] = -0.5;
    squarewave[n + 2 * WAVETABLE_SIZE / 4] = 0.5;
    squarewave[n + 3 * WAVETABLE_SIZE / 4] = -0.5;
  }

  //
  //	This sets up NUM_OSCILLATORS (16) simulataneous waveform oscillators
  //
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    oscillator[i].output_ptr = &(output_samples[i][0]);
    oscillator[i].samples_per_block = SAMPLE_BLOCKSIZE;

    oscillator[i].phase = 0;
    oscillator[i].phaseIncrement = 0;
    oscillator[i].frequencyIncrement = 0;

    oscillator[i].num_fractionalBits = 32 - LOG2_WAVETABLE_SIZE;
    oscillator[i].mask_fractionalBits =
        (0x00000001L << (32 - LOG2_WAVETABLE_SIZE)) - 1;
    oscillator[i].mask_waveIndex = WAVETABLE_SIZE - 1;
    oscillator[i].scaler_fractionalBits =
        ((float)WAVETABLE_SIZE) / BIT32_NORMALIZATION;

    oscillator[i].fadeDim1 = 0.0f;
    oscillator[i].fadeDim1Increment = 0.0f;

    oscillator[i].wave000 = sinewave;
    oscillator[i].wave001 = sawtooth;
    oscillator[i].wave010 = silence;
    oscillator[i].wave011 = squarewave;
    oscillator[i].wave100 = squarewave;
    oscillator[i].wave101 = sawtooth;
    oscillator[i].wave110 = silence;
    oscillator[i].wave111 = silence;
  }
  return &oscillator[0];
}
