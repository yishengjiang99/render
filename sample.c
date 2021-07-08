typedef unsigned int uint32_t;
float hermite4(float frac_pos, float xm1, float x0, float x1, float x2) {
  const float c = (x1 - xm1) * 0.5f;
  const float v = x0 - x1;
  const float w = c + v;
  const float a = w + v + (x2 - x0) * 0.5f;
  const float b_neg = w + a;
  return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

void upsample_wave_table(uint32_t inputN, uint32_t outputN, float* inputArr, float*output,float ratio) {
float frac = 0.0f;
unsigned int pos = 0;
  for (unsigned int i = 0; pos<inputN-4 && i<outputN-1; i++) {
    float fm1 = i > 0 ? *(inputArr + pos - 1) : 0;
    float f1 = *(inputArr+pos);
    float f2 = *(inputArr+pos + 1);
    float f3 = *(inputArr+pos + 2);
    output[i] = hermite4(frac, fm1, f1, f2, f3);
  frac += ratio;
    if(frac >= 1.0f) {
     frac--;
     pos++;
    }
    if(pos>=inputN-4) pos = 1;
  
  }
}