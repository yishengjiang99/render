#include "sf2.c"
#include <assert.h>
#include "presetexp.c"
int main()
{
	readsf(fopen("file.sf2", "rb"));
	preset *p = IndexPresets();
	preset *h = p;

	printz(h); //0, 0, 55, 33h); ///p, 0);
}