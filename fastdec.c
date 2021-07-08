#include <stdio.h>
#define sampleRate 48000
#include "math.h"

  
#define silence pow(10,-1440.0/200.0f)
  
  // double coeff = -144.0 / 20.0f / attck;

  //  void init(float levelBegin, float levelEnd, float releaseTime) {
  //      currentLevel = levelBegin;
  //      coeff = (log(levelEnd) - log(levelBegin)) /
  //              (releaseTime * sampleRate);
  //  }
int main() {
  double attck = pow(2.0f, -6700.0 / 1200.0)*sampleRate;
  double currentLevel;
  
  double start = silence;
  double peak = pow(10.0, -90/200.f);

  currentLevel=start;
  double coeff = -90.0f/200.f +1440.f/200.f;

  for (int i = 0; i < attck; i++) {
    printf("\n%.13f %f %d", currentLevel += coeff * currentLevel, coeff, i);
  }
}