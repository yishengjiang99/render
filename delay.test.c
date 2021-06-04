#include "delay.c"
#include <math.h>

#include <assert.h>
int main()
{
	ASSERT_NEAR(calcratio(&z, &sh, 54), 1.f, .1f);
	z.OverrideRootKey = -1;
	sh.originalPitch = 58;

	z.FineTune = 10;
}