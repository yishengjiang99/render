#include <gtest/gtest.h>
#include <math.h>
extern "C" {
#include "../libs/biquad.c"
#include "../runtime.c"
#include "../sf2.c"
#include "../tictok.c"
}
class CtxTest : public ::testing::Test {
 protected:
  void SetUp() override {
    readsf(fopen("../FluidR3_GM.sf2", "rb"));
    init_ctx();
    setProgram(2, 60);
  }
};
// Demonstrate some basic assertions.
TEST_F(CtxTest, BasicAssertions) {
  EXPECT_NE(&phdrs[0], nullptr);
  EXPECT_NE(&pbags[0], nullptr);
  EXPECT_NO_FATAL_FAILURE(findPresetZonesCount(0));
  EXPECT_NO_FATAL_FAILURE(findPresetZones(0, 2));
  EXPECT_EQ(pbags[0].pgen_id, 0);
  short at[60] = defattrs;
  zone_t *z = (zone_t *)at;
  ASSERT_EQ(z->ModEnvDelay, -12000);
  ASSERT_EQ(z->OverrideRootKey, -1);
  ASSERT_EQ(z->ModEnvRelease, -12000);
}
TEST_F(CtxTest, defaultattrs) {
  short attrs[60] = defattrs;
  zone_t *z = (zone_t *)attrs;

  EXPECT_EQ(z->OverrideRootKey, -1);
}
#define CHECK(msg, ...) \
  do {                  \
    SCOPED_TRACE(msg);  \
    check(__VA_ARGS__); \
  } while (0)
// Demonstrate some basic assertions.
TEST_F(CtxTest, basicRuntime) {
  SCOPED_TRACE("100");

  EXPECT_NE(g_ctx, nullptr);
  setProgram(0, 0);
  EXPECT_EQ(g_ctx->channels[0].pzset.hdr.pid, 0);
  ASSERT_NO_THROW(noteOn(0, 77, 99, 0));
  ASSERT_NE(g_ctx->channels[0].voices, nullptr);
}

TEST_F(CtxTest, writeTofile) {
  setProgram(0, 0);
  noteOn(0, 77, 99, 0);

  g_ctx->outputFD = fopen("t.pcm", "w");

  EXPECT_NO_THROW(render_fordr(g_ctx, 1, nullptr));
  pclose(g_ctx->outputFD);
}
TEST_F(CtxTest, drums) {
  g_ctx->outputFD = fopen("tdr.pcm", "w");

  setProgram(9, 0);
  EXPECT_EQ(g_ctx->channels[9].program_number, 0);
  noteOn(9, 77, 54, 0);
  EXPECT_NO_THROW(render_fordr(g_ctx, 1, nullptr));

  pclose(g_ctx->outputFD);
}
TEST_F(CtxTest, BEnch) {
  for (int i = 0; i < nphdrs; i++) {
    TIC()

    ASSERT_NO_FATAL_FAILURE(findPresetZones(i, findPresetZonesCount(i)));
    TOK()
    ASSERT_LT(tiktoktime(), 10e7);
  }
  PresetZones tr = findPresetByName("Trumpet");
  ASSERT_NE(tr.zones, nullptr);
}
TEST_F(CtxTest, findpresetZones) {
  for (int i = 0; i < nphdrs; i++) {
    if (phdrs[i].bankId != 0) continue;
    setProgram(2, phdrs[i].pid);
    ASSERT_NE(g_ctx->channels[2].pzset.zones, nullptr);
    ASSERT_GT(g_ctx->channels[2].pzset.npresets, 0);

    for (int j = 0; j < 125; j++) {
      ASSERT_GT(get_sf(2, j, 55), 0);
    }
  }
}

TEST_F(CtxTest, lpf) {
  zone_t z;
  z.FilterFc = 6900;  // unit is midi octave val middle c or something
  // voice*v = newVoice(&z, 69,33,0);
  // ASSERT_NE(v->lpf,nullptr);
  // ASSERT_EQ(v->lpf->sample_rate, g_ctx->sampleRate);
  // 	ASSERT_FLOAT_EQ(v->lpf->cutoff_freq, 440.0f);
  // 	ASSERT_GT(process_input(v->lpf, 0.8), 0);
  // 			ASSERT_NE(v->lpf->m1, 0);
}
