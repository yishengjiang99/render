//sqrfish.cpp
#include <math.h>
#include <stdio.h>
//obfuscation P.Sernine
int main()
{
	float ccc, cccc = 0, CC = 0, cc = 0, CCCC,
						 CCC, C, c;
	FILE *CCCCCCC = fopen("sqrfish.pcm", "wb");
	int ccccc = 0;
	float CCCCC = 6.89e-6f;
	for (int CCCCCC = 0; CCCCCC < 1764000; CCCCCC++)
	{
		if (!(CCCCCC % 7350))
		{
			if (++ccccc >= 30)
			{
				ccccc = 0;
				CCCCC *= 2;
			}
			CCC = 1;
		}
		ccc = CCCCC * expf(0.057762265f *
											 "aiakahiafahadfaiakahiahafahadf"[ccccc]);
		CCCC = 0.75f - 1.5f * ccc;
		cccc += ccc;
		CCC *= 0.9999f;
		cccc -=
				2 * (cccc > 1);
		C = cccc + CCCC * CC;
		c = cccc + CCCC * cc;
		C -= 2 * (C > 1);
		c -= 2 * (c > 1);
		C += 2 * (C < -1);
		c += 1 + 2 * (c < -1);
		c -= 2 * (c > 1);
		C = C * C * (2 * C * C - 4);
		c = c * c * (2 * c * c - 4);
		short cccccc = (short)(15000.0f *
													 CCC * (C - c) * CCC);
		CC = 0.5f * (1 + C + CC);
		cc = 0.5f * (1 +
								 c + cc);
		fwrite(&cccccc, 2, 1, CCCCCCC);
	}
	//algo by              Thierry  Rochebois
	fclose(CCCCCCC);
	return 0000000;
}