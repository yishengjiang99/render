#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/fft.c"
#include "libs/wavetable_oscillator.c"
#define output_sampleRate 4096
#define dspbuffersize 4096
#include "call_ffp.c"
#include "runtime.c"

#define FFTBINS 4096
#define sampleVoice(v, c, fo)                            \
  {                                                      \
    bzero(g_ctx->outputbuffer, sizeof(float) * FFTBINS); \
    loop(v, g_ctx->outputbuffer, g_ctx->channels[0]);    \
    bzero(c, FFTBINS * sizeof(complex));                 \
    for (int k = 0; k < FFTBINS; k++) {                  \
      c[k].real = g_ctx->outputbuffer[k];                \
      c[k].imag = 0.0f;                                  \
    }                                                    \
    FFT(c, log2(FFTBINS), stbl);                         \
    fwrite(c, sizeof(complex), FFTBINS, fo);             \
    fflush(fo);                                          \
  }

void render_and_fft(complex* c, double* stbl, float* destination) {
  bzero(c, sizeof(complex) * 2 * FFTBINS);

  render(g_ctx);
  for (int i = 0; i < FFTBINS; i++) {
    c[i].real = g_ctx->outputbuffer[i * 2];
    c[i].imag = 0.0f;
  }
  FFT(c, log2(FFTBINS), stbl);
  bit_reverse(c, log2(FFTBINS));
  for (int i = 0; i < FFTBINS; i++) {
    // printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);

    if (i > 8 && i < FFTBINS / 2) {
      c[i].real = 0.0f;
      c[i].imag = 0.0f;
    }
    if (i > FFTBINS / 2 + 8) {
      c[i].real = 0.0f;
      c[i].imag = 0.0f;
    }
  }

  bit_reverse(c, log2(FFTBINS));
  iFFT(c, log2(FFTBINS), stbl);
  for (int i = 0; i < FFTBINS; i++) {
    /// printf("\n%d: %f, %f", i, (float)c[i].real, (float)c[i].imag);
    destination[i] = (float)c[i].real;
  }
}
int main() {
  init_oscillators();
  double sinetable[FFTBINS << 2];
  memcpy(sinetable, stbl, (FFTBINS << 2) * sizeof(double));
  char* file = "FluidR3_GM.sf2";
  readsf(fopen(file, "rb"));
  complex c[FFTBINS];
  init_ctx();
  setProgram(0, 0);
  int vel = 55;
  int keyPressed = 1;
  int tableIndex = 0;
  voice *v1, *v2;
  FILE* o = ffp(2, 48000);

  for (int midi = 21; midi < 90; midi += 10) {
    g_ctx->channels[0].voices = NULL;
    noteOn(0, midi, vel, 0);
    voice* v1 = g_ctx->channels[0].voices;
    if (v1->next) v2 = v1->next;
    shdrcast* sh = (shdrcast*)(shdrs + v1->z->SampleId);
    adsr_t* env = v1->ampvol;
    float decay_sec = powf(2.0f, v1->z->VolEnvDecay / 1200.0f);
    float release_sec = powf(2.0f, v1->z->VolEnvRelease / 1200.0f);
    float sustain = v1->z->VolEnvSustain / 1000.0f;
    float att_sec = powf(2.0f, v1->z->VolEnvAttack / 1200.0f);

    float mod_att = powf(2.0f, v1->z->ModEnvAttack / 1200.0f);
    float mod_release = powf(2.0f, v1->z->ModEnvRelease / 1200.0f);
    float modDEc = powf(2.0f, v1->z->ModEnvDecay / 1200.0f);
    float modf_sustain = v1->z->ModEnvSustain / 1000.0f;

    render_and_fft(c, stbl, sampleTableRef(tableIndex++));
    while (env->att_steps > 0) envShift(env);
    render_and_fft(c, stbl, sampleTableRef(tableIndex++));

    env->db_attenuate = 0;
    env->release_rate = 0;
    if (v1->z->ModEnv2FilterFc) {
      v1->lpf = BiQuad_new(LPF, v1->z->FilterQ / 10.0f,
                           p2over1200(v1->z->FilterFc + v1->z->ModEnv2FilterFc),
                           g_ctx->sampleRate, 1.0);
    }
    if (v1->z->ModEnv2Pitch) {
      v1->ratio =
          calcratio(v1->z, sh, v1->midi) * p2over1200(v1->z->ModEnv2Pitch);
    }
    render_and_fft(c, stbl, sampleTableRef(tableIndex++));
    if (v1->z->ModEnv2FilterFc) {
      v1->lpf = BiQuad_new(LPF, v1->z->FilterQ / 10.0f,
                           p2over1200(v1->z->FilterFc + v1->z->ModEnv2FilterFc),
                           g_ctx->sampleRate, 1.0);
    }
    if (v1->z->ModEnv2Pitch) {
      v1->ratio =
          calcratio(v1->z, sh, v1->midi) * p2over1200(v1->z->ModEnv2Pitch);
    }

    render_and_fft(c, stbl, sampleTableRef(tableIndex++));

    for (int k = 0; k < 9; k += 3) {
      int keyPressed = 1;
      int t = 0;
      set_midi(0, midi + k);
      oscillator->fadeDim1 = 0.f;
      oscillator->fadeDim1Increment = 1.0f / (SAMPLE_RATE * att_sec);
      oscillator->wave000 = &sample_tables[0];
      oscillator->wave001 = &sample_tables[1];
      while (oscillator->fadeDim1 < 1.0f) {
        t += oscillator->samples_per_block;

        wavetable_1dimensional_oscillator(oscillator);
        fwrite(oscillator->output_ptr, sizeof(float),
               oscillator->samples_per_block, o);
      }

      oscillator->fadeDim1 = 0.f;
      oscillator->fadeDim1Increment =
          1.0f * sustain / (SAMPLE_RATE * decay_sec);
      oscillator->wave000 = &sample_tables[1];
      oscillator->wave001 = silence;
      while (keyPressed == 1 && oscillator->fadeDim1 < 1.0f) {
        wavetable_1dimensional_oscillator(oscillator);
        t += oscillator->samples_per_block;
        fwrite(oscillator->output_ptr, sizeof(float),
               oscillator->samples_per_block, o);
        if (oscillator->fadeDim1 > .1f) keyPressed = 0;
      }
      noteOff(0, midi);

      oscillator->fadeDim1Increment = 1.0f / (SAMPLE_RATE * release_sec);
      while (oscillator->fadeDim1 < 1.0f && t < 48000) {
        wavetable_1dimensional_oscillator(oscillator);
        t += oscillator->samples_per_block;

        fwrite(oscillator->output_ptr, sizeof(float),
               oscillator->samples_per_block, o);
        printf("%f", oscillator->fadeDim1);
      }
    }
  }
  pclose(o);
  return 0;
}