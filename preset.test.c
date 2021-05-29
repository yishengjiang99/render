#include "sf2.c"
#include <assert.h>
#include "presetexp.c"
int main()
{
	readsf(fopen("file.sf2", "rb"));
	preset *p = IndexPresets();
	preset *h = p;

	dfprintff(0, 0, 55, 33); ///p, 0);
}