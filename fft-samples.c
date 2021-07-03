

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

  char* sf2file = "static/GeneralUserGS.sf2";
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
    
    zone_t* z = pz->zones + i;
    if(z->KeyRange.hi-z->KeyRange.lo>23) continue;
    shdrcast* sh = (shdrcast*)(shdrs + z->SampleId);
    g_ctx->sampleRate = sh->sampleRate;
    voice* v = newVoice(z, 40, 77, cid);  // z->VelRange.hi, cid);
    float output[4096];
    for (int o = 0; o < 48000; o += 128) {
      loop(v, output, g_ctx->channels[0]);
      fwrite(output, sizeof(float), 128, stdout);
      //usleep(1e3*2.5);
    }

  }
}
