#include <gtest/gtest.h>

extern "C"
{

#include "../sf2.c"
#include "../preset.c"
}
class CtxTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		readsf(fopen("sm.sf2", "rb"));

	}
};
// Demonstrate some basic assertions.
TEST_F(CtxTest, BasicAssertions)
{

	EXPECT_NE(&phdrs[0], nullptr);
}