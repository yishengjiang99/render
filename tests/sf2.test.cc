#include <gtest/gtest.h>
#include <math.h>
extern "C" {
#include "../sf2/sf2.c"
}

class CtxTest : public ::testing::Test {
 protected:
  void SetUp() override { readsf("../file.sf2"); }
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

TEST_F(CtxTest, BEnch) {
  for (int i = 0; i < nphdrs; i++) {
    ASSERT_NO_FATAL_FAILURE(findPresetZones(i, findPresetZonesCount(i)));
  }
  PresetZones *tr = findPresetByName("Trumpet");
  ASSERT_NE(tr->zones, nullptr);
}

TEST_F(CtxTest, lpf) {
  zone_t z;
  z.FilterFc = 6900;  // unit is midi octave val middle c or something
  // voice *v = newVoice(&z, 69, 33, 0);
  // ASSERT_NE(v->lpf, nullptr);
  // ASSERT_EQ(v->lpf->sample_rate, g_ctx->sampleRate);
  // ASSERT_FLOAT_EQ(v->lpf->cutoff_freq, 440.0f);
  // ASSERT_GT(process_input(v->lpf, 0.8), 0);
  // ASSERT_NE(v->lpf->m1, 0);
}
