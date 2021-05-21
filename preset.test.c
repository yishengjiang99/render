#include "sf2.c"
#include <assert.h>
static char *testcase
#define describe(str) \
	{                   \
		testcase = str;   \
		printf(str);      \
	}

		int
		main()
{
	readsf(fopen("file.sf2", "rb"));
	IndexPresets();
	dfprint(psets, 0);

	// preset *p[4];
	// dfprintff(0, 0, 55, 33); ///p, 0);
}