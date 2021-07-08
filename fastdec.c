#include <stdio.h>
#define sampleRate 48000
#include "math.h"


float coeff;
float currentLevel;

void init(float levelBegin, float levelEnd, float releaseTime) {
  currentLevel = levelBegin;
  coeff = (log(levelEnd) - log(levelBegin)) / (releaseTime * sampleRate);
}

inline void calculateEnvelope(int samplePoints) {
  for (int i = 0; i < samplePoints; i++) {
    currentLevel += coeff * currentLevel;
    printf("\n%f", currentLevel);
  }
}

#define silence pow(10,-1440.0/200.0f)
  

int main() {
  double attck = pow(2.0f, -6700.0 / 1200.0)*sampleRate;
  double currentLevel;
  init(silence, 1, attck)
  
  double start = silence;
  double peak = pow(10.0, -90/200.f);

  double coeff = -90.0f/200.f +1440.f/200.f;

  for (int i = 0; i < attck; i++) {
    printf("\n%.13f %f %d", currentLevel += coeff * currentLevel, coeff, i);
  }
}