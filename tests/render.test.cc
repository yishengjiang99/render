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

TEST_F(Ff, basic) { ASSERT_TRUE(true); }
