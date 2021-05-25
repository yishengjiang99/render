#include <gtest/gtest.h>

extern "C"
{
#include "ctx.c"
}
class CtxTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		readsf(fopen("file.sf2", "rb"));
		ctx = init_ctx();
		noteOn(ctx, 0, 55, 55, 0);
	}

	ctx_t *ctx;
};
// Demonstrate some basic assertions.
TEST_F(CtxTest, BasicAssertions)
{
	readsf(fopen("file.sf2", "rb"));

	EXPECT_NE(&phdrs[0], nullptr);
	EXPECT_EQ(ctx->channels[0].program_number, 0);
	//	EXPECT_NE(ctx->voices[0].z, nullptr);
}