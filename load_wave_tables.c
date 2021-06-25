
/**
 * #include <stdint.h>
#define NUM_OSCILLATORS 16
#define SAMPLE_BLOCKSIZE 128

#define MASK_FRACTIONAL_BITS 0x000FFFFF
#define MASK_WAVEINDEX 0x00000FFFUL
#define WAVETABLE_SIZE 4096
#define LOG2_WAVETABLE_SIZE 12

#define PIF 3.1415926539f
#define BIT32_NORMALIZATION 4294967296.0f
#define SAMPLE_RATE 48000.0f
//
*/
#define N_TABLE_BITS 5
#define WAVETABLE_SIZE (1 << N_TABLE_BITS )
#define MASK_WAVEINDEX 0x000000FFUL
#define MAST_FRACTIONAL_BITS 0x00FFFFFF
#define ONELONE (float)((1L<<32)-0.01f)

typedef struct _frame{
  int duration_nsamples;
  float wavetable[WAVETABLE_SIZE];
} wavetable_frame;
#include<stdio.h>
#include<stdlib.h>

int main(){

   // printf("%f", ONELONE-.1f);
    system("chrome-cli open 'https://dsp.grepawk.com/render/outside.html'");
    //    system("chrome-cli open 'https://dsp.grepawk.com/render/outside.html'");

system("chrome-cli execute 'alert(\"hi\")'");
    // char buf[1024];
    // fread(buf,1024,1,proc);
    // fwrite(buf,1024,1,stdout);

} emcc readriff_standalone.c -s EXPORTED_FUNCTIONS=["readRiff"] -o readriff.js
