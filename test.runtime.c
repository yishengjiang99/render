
#include <assert.h>

#include "call_ffp.c"
#include "runtime.c"
#include "sf2.c"
void cb(ctx_t *ctx) {
  for (int i = 0; i < 128; i++) printf("\n%f", ctx->outputbuffer[0]);
}
int main() {
  init_ctx();
  FILE *f = fopen("GeneralUserGS.sf2", "rb");
  if (!f) perror("oaffdf");

  readsf(f);

  g_ctx->outputFD = ffp(2, 48000);
  g_ctx->mastVol = 1.0f;

  setProgram(0, 0);

  for (int m = 33; m < 78; m++) {
    noteOn(0, m, 6 * 10, 0);

    render_fordr(g_ctx, .5, NULL);
    noteOff(0, m);
    render_fordr(g_ctx, .5, NULL);
  }
  fclose(g_ctx->outputFD);
}