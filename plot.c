
#include "gentypes.c"
#include "sf2.c"
int chart()
{
	pbag *pb = pbags;
	for (int i = 0; i < npgens; i++)
	{

		printf("\t%s %d %d", generator[pgens[i].genid], pgens[i].genid, pgens[i].val.shAmount);
		if (i >= (pb + 1)->pgen_id)
		{
			pb++;
			printf("\r\n");
		}
	}

	ibag *ib = ibags;
	for (int i = 0; i < nigens - 1 && ib != ibags + nibags - 1; i++)
	{

		if (i >= (ib + 1)->igen_id)
		{
			ib++;
			printf("};\n%d={", ib - ibags);
		}
		printf("\n%d:\t%d,\t \/\/%s", igens[i].genid, igens[i].val.uAmount, generator[igens[i].genid]);
	}
}
int smain()
{
	readsf(fopen("file.sf2", "rb"));
	chart();
	return 0;
}
