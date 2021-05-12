
#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <string.h>
#include "sf2.h"
#ifndef voice_h
#include "voice.c"

#endif
#ifndef listscan
#include "listscan.c"

#endif
#define sr 48000

int readsf(FILE *fd)
{
	header_t *header = (header_t *)malloc(sizeof(header_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(header_t), 1, fd);

	printf("%.4s %.4s %.4s %u\n", header->name, header->sfbk, header->list, header->size);
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

	printf("readdone");
	return 1;
}

void get_sf(int pid, int bkid, int key, int vel, node **head, int index)
{
	short *attributes;
	short igset[60] = {-1};
	int instID = -1;
	int lastSampId = -1;
	short pgdef[60] = {-1};
	for (int i = 0; i < nphdrs - 1; i++)
	{
		if (phdrs[i].bankId != bkid || phdrs[i].pid != pid)
			continue;
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
				short igdef[60] = {-1};
				for (int ibg = ibgId; ibg < lastibg; ibg++)
				{
					lastSampId = -1;
					short igset[60] = {-1};
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
							if (igset[i] != -1)
								*(attributes + i) = igset[i];
							else if (igdef[i] != -1)
								*(attributes + i) = igdef[i];
							else
							{
								*(attributes + i) = 0;
							}

							if (pgset[i] != -1)
								*(attributes + i) += pgset[i];
							else if (pgdef[i] != -1)
								*(attributes + i) += pgdef[i];
						}
						zone_t *z = (zone_t *)attributes;
						voice *v = newVoice(z, key, vel);

						insert_node(head, newNode(v, index));
					}
				}
			}
		}
	}
	//return head;
}
