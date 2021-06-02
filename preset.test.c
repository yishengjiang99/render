#include "sf2.c"
#include <assert.h>
#include "runtime.c"
int main()
{
	readsf(fopen("file.sf2", "rb"));
	phdr pdr = findPreset("Trumpet");
	printf("%d", pdr.pid);

	//printz(h); //0, 0, 55, 33h); ///p, 0);
}