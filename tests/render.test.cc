#include <gtest/gtest.h>
#include <math.h>
extern "C"
{
#include "../sf2.c"
#include "../runtime.c"
#define TML_IMPLEMENTATION
#include "../tml.h"
	static tml_message *m;
}
class Ff : public ::testing::Test
{

	void SetUp() override
	{
		readsf(fopen("../GeneralUserGS.sf2", "rb"));
		init_ctx();
		m = tml_load_filename("./song.mid");
	}
};

TEST_F(Ff, basic)
{
	ASSERT_TRUE(true);
}