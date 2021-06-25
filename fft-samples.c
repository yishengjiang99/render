

#include "fft-samples.h"

#define output_sampleRate 48000
#define dspbuffersize 128
#include <strings.h>

#include "libs/biquad.c"
#include "libs/fft.c"
#include "libs/wavetable_oscillator.c"
#include "macros.h"
#include "runtime/runtime.c"
#include "sf2/sf2.c"

#ifndef SAMPLE_RATE  // the other one is the rendering engine samp rate
#define SAMPLE_RATE 48000

#endif

void init_detailed_wavtables(int pid, int bankid);
void render_and_fft(voice* v, complex* c, double* stbl, float*frame);
#define FFTBINS 32
static double stbl[FFTBINS / 4];

typedef struct _frame{
  int duration_nsamples;
  complex frequency_domain_real_img[FFTBINS];
} wavetable_frame;

#define FFTBINS 32
#define LOG2_FFTBINS 5
int main(int argc, char** argv) {
  sin_table(stbl, log2(FFTBINS));

  wavetable_oscillator_data* osc = malloc(sizeof(wavetable_oscillator_data));


  char* sf2file = argc > 1 ? argv[1] : "file.sf2";
  char outfile[1024];
  double sinetable[FFTBINS << 2];
  memcpy(sinetable, stbl, (FFTBINS << 2) * sizeof(double));
  readsf(sf2file);
  init_ctx();
  for (int i = 0; i < nphdrs; i++) {
    int pid = phdrs[i].pid;
    int bid = phdrs[i].bankId;
    init_detailed_wavtables(pid, bid);
    break;
  }
  return 1;
}
void init_detailed_wavtables(int pid, int bankid) {
  int cid = bankid == 128 ? 9 : 0;
  setProgram(cid, pid);
  PresetZones *pz = presetZones+pid;
  complex c[FFTBINS];
  
  for (int i = 0; i < pz->npresets - 1; i++) {
    zone_t *z =pz->zones + i;
    shdrcast* sh = (shdrcast*)(shdrs + z->SampleId);
    g_ctx->sampleRate = sh->sampleRate;
    voice* v = newVoice(z, z->KeyRange.hi, z->VelRange.hi, cid);
    wavetable_frame *frames = malloc(sizeof(wavetable_frame)*10);
    wavetable_frame** tr = &frames;

    for (int f = 0; f < 110; f++) {
      // g_ctx->outputbuffer = (*tr)->frequency_domain_real_img;
      loopreal(v, (double*)(*tr)->frequency_domain_real_img);
       FFT((*tr)->frequency_domain_real_img, log2(FFTBINS), stbl);
       bit_reverse((*tr)->frequency_domain_real_img,log2(FFTBINS));
       tr = tr+1;
    }
  }

}
