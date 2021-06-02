#include <gtest/gtest.h>

extern "C"
{
#include "../runtime.c"
#include "../tictok.c"
}
class CtxTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		readsf(fopen("../GeneralUserGS.sf2", "rb"));
		init_ctx();
	}
};
// Demonstrate some basic assertions.
TEST_F(CtxTest, BasicAssertions)
{
	EXPECT_NE(&phdrs[0], nullptr);
	EXPECT_NE(&pbags[0], nullptr);
	EXPECT_NO_FATAL_FAILURE(findPresetZonesCount(0));
	EXPECT_NO_FATAL_FAILURE(findPresetZones(0, 2));

	EXPECT_EQ(pbags[0].pgen_id, 0);
}
// Demonstrate some basic assertions.

TEST_F(CtxTest, basicbemchj)
{

	for (int i = 0; i < nphdrs; i++)
	{
		TIC()

		ASSERT_NO_FATAL_FAILURE(findPresetZones(i, findPresetZonesCount(i)));
		TOK()
		ASSERT_LT(tiktoktime(), 10e7);
	}
}