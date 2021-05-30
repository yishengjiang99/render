#include "sf2.c"
#include "delay.c"
#include "stbl.c"
#include "libs/fft.c"
#include <assert.h>

int main(int argc, char const *argv[])
{
	readsf(fopen("file.sf2", "rb"));
}
