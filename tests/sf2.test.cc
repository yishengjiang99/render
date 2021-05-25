#include <gtest/gtest.h>

extern "C"
{

#include "../sf2.c"
#include "../presetexp.c"
}
class CtxTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		readsf(fopen("../sm.sf2", "rb"));
		presets = IndexPresets();
		l1 = presets->link;
	}
	preset *presets, *l1;
};
// Demonstrate some basic assertions.
TEST_F(CtxTest, BasicAssertions)
{
	EXPECT_NE(&phdrs[0], nullptr);
	EXPECT_EQ(presets->hdr.pid, 0);
}