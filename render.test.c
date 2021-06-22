#include <assert.h>
#include <stdio.h>

#include "runtime/runtime.h"
#include "sf2/sf2.h"

int main() {
  short attrs[60] = defattrs;
  zone_t *z = (zone_t *)attrs;
  printf("%d,%d", z->VelRange.lo, z->VelRange.hi);

  init_ctx();
  readsf("file.sf2");
  ctx_t *ctx = init_ctx();
  ctx->outputFD = fopen("o.pcm", "w");
  ctx->channels[0].program_number = 60;
  ctx->channels[1].program_number = 0;
  setProgram(0, 0);
  noteOn(0, 77, 99, 0);

  render_fordr(g_ctx, 1.0, NULL);

  return 1;

  // assert(calcratio(&z, &sh, 55) == powf(2.0f, (71.0f - 55) / 1200.0f));
}
