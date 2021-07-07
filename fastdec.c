#include "includes/read.ccxx                                                                                           "
float coeff;
float currentLevel;
#define BIT32_NORMALIZATION 4294967296.0f

typedef struct ed {
  float decibel_1, decibel_2, duration;
  float coeff, currentLevel;
  int sampleRate;
} exponential_decay;

typedef struct {
  uint32_t delay_steps, hold_steps;  //, decay_steps, release_steps;
  exponential_decay *attack, *decay, *release;
  float egval;
} adsr_t;

adsr_t *zoneVolAmp(zone_t *zone, int sampleRate) {
  newEnv(zone->VolEnvDelay, zone->VolEnvAttack, zone->VolEnvHold,
         zone->VolEnvDecay, zone->VolEnvRelease, zone->VolEnvSustain,
         zone->Attenuation, sampleRate);
}

int main() {
  init(1, 0, 222);
  calculateEnvelope(100);
}