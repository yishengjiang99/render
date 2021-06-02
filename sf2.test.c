#include "sf2.c"
#include "delay.c"
#include "stbl.c"
#include "libs/fft.c"
#include <assert.h>

int main(int argc, char const *argv[])
{
	readsf(fopen("GeneralUserGS.sf2", "rb"));
	for (int i = 0; i < nphdrs; i++)
	{
		//	printf("\n%s: %d", phdrs[i].name, presetZones[i].npresets);
		for (int j = 0; j < presetZones[i].npresets; j++)
		{
			printf("\n%hd %d,", presetZones[i].zones[j].VelRange, presetZones[i].zones[j].KeyRange & 0x7f);
			// /		printf("\n%hd", presetZones[i].zones[j].VelRange);
		}
	}
}
