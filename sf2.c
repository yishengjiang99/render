#ifndef SF2_C
#define SF2_C

#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <string.h>
#include "sf2.h"
#include "voice.c"

#define sr 24000
int readsf(FILE *fd)
{
	sheader_t *header = (sheader_t *)malloc(sizeof(sheader_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(sheader_t), 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	info = malloc(h2->size);
	fread(info, h2->size, 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	data = (short *)malloc(h2->size / 2 * sizeof(short));
	sdta = (float *)malloc(h2->size / 2 * sizeof(float));
	float *trace = sdta;
	nsamples = h2->size / sizeof(short);

	printf("\n\t %ld", ftell(fd));
	fread(data, sizeof(short), nsamples, fd);

	for (int i = 0; i < nsamples; i++)
	{
		*trace++ = *(data + i) / 32767.0f;
	}
	//free(data);
	FILE *cache = fopen("cache.dat", "wb");

#define readSection(section)                  \
	fread(sh, sizeof(section_header), 1, fd);   \
	printf("%.4s:%u\n", sh->name, sh->size);    \
	n##section##s = sh->size / sizeof(section); \
	section##s = (section *)malloc(sh->size);   \
	fread(section##s, sizeof(section), sh->size / sizeof(section), fd);

	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(h2, sizeof(header2_t), 1, fd);
	printf("%.4s %u \n", h2->name, h2->size);

	readSection(phdr);
	readSection(pbag);
	readSection(pmod);
	readSection(pgen);
	readSection(inst);
	readSection(ibag);
	readSection(imod);
	readSection(igen);
	readSection(shdr);
	return 1;
}

zone_t *get_sf(int pid, int bkid, int key, int vel)
{
	zone_t *z;
	short *attributes;
	short igset[60] = {-1};
	int instID = -1;
	int lastSampId = -1;
	short pgdef[60] = {-1};
	for (int i = 0; i < nphdrs - 1; i++)
	{
		if (phdrs[i].bankId != bkid || phdrs[i].pid != pid)
			continue;
		//	printf("\n%s %d", phdrs[i].name, key);
		int lastbag = phdrs[i + 1].pbagNdx;
		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;
			short pgset[60] = {-1};
			instID = -1;
			for (int k = pgenId; k < lastPgenId; k++)
			{
				pgen_t *g = pgens + k;

				if (vel > -1 && g->operator== 44 &&(g->val.ranges.lo > vel || g->val.ranges.hi < vel))
					break;
				if (key > -1 && g->operator== 43 &&(g->val.ranges.lo > key || g->val.ranges.hi < key))
					break;
				if (g->operator== 41)
				{
					instID = g->val.uAmount;
				}
				pgset[g->operator] = g->val.shAmount;
			}
			if (instID == -1)
			{
				memcpy(pgdef, pgset, sizeof(pgset));
			}
			else
			{
				inst *ihead = insts + instID;
				int ibgId = ihead->ibagNdx;
				int lastibg = (ihead + 1)->ibagNdx;
				short igdef[60];
				for (int ibg = ibgId; ibg < lastibg; ibg++)
				{
					lastSampId = -1;
					short igset[60];
					ibag *ibgg = ibags + ibg;
					pgen_t *lastig = ibg < nibags - 1 ? igens + (ibgg + 1)->igen_id : igens + nigens - 1;
					for (pgen_t *g = igens + ibgg->igen_id; g->operator!= 60 && g != lastig; g++)
					{

						if (vel > -1 && g->operator== 44 &&(g->val.ranges.lo > vel || g->val.ranges.hi < vel))
							break;
						if (key > -1 && g->operator== 43 &&(g->val.ranges.lo > key || g->val.ranges.hi < key))
							break;
						igset[g->operator]=g->val.shAmount;
						if (g->operator== 53)
						{
							lastSampId = g->val.shAmount; // | (ig->val.ranges.hi << 8);
						}
					}
					if (lastSampId > -1)
					{
						attributes = (short *)malloc(sizeof(short) * 60);
						for (int i = 0; i < 60; i++)
						{
							if (igset[i] != 0)
								*(attributes + i) = igset[i];
							else if (igdef[i] != 0)
								*(attributes + i) = igdef[i];
							else
							{
								*(attributes + i) = 0;
							}

							if (pgset[i] != 0)
								*(attributes + i) += pgset[i];
							else if (pgdef[i] != 0)
								*(attributes + i) += pgdef[i];
						}
						z = (zone_t *)attributes;
					}
				}
			}
		}
	}
	//return head;
	return z;
}

#endif