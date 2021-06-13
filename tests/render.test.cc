#include <gtest/gtest.h>
#include <math.h>

extern "C" {
#include "../runtime.c"
#include "../sf2.c"
}

class Ff : public ::testing::Test {
  void SetUp() override {
    readsf(fopen("../GeneralUserGS.sf2", "rb"));
    init_ctx();
  }
};

TEST_F(Ff, LookUPTAbles) {
  for (int i = -12000; i < 20000; i++) {
    ASSERT_GT(p2over1200(i), p2over1200(i - 1));
    ASSERT_NE(p2over1200(i), INFINITY);
    ASSERT_FLOAT_EQ(p2over1200(1200), 2.0);
  }
}

TEST_F(Ff, newVoice) {
  zone_t z = findByPid(60, 0).zones[0];
  voice* v = newVoice(&z, 66, 99, 0);
  ASSERT_NE(v, nullptr);
  ASSERT_NE(v->lpf, nullptr);
  ASSERT_LE(z.VelRange.lo, z.VelRange.hi);
  for (zone_t* z = findByPid(60, 0).zones; z != NULL; z++) {
    ASSERT_NO_THROW(newVoice(z, z->KeyRange.hi - 1, z->VelRange.hi - 1, 0));
  }
}
