
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
	sheader_t *header = (sheader_t *)malloc(sizeof(sheader_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(sheader_t), 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	//f("\n%.4s %u", h2->name, h2->size);
	info = malloc(h2->size);
	fread(info, h2->size, 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	//f("\n%.4s %u", h2->name, h2->size);
	data = (short *)malloc(h2->size / 2 * sizeof(short));
	sdta = (float *)malloc(h2->size / 2 * sizeof(float));
	float *trace = sdta;
	nsamples = h2->size / sizeof(short);

	//f("\n\t %ld", ftell(fd));
	fread(data, sizeof(short), nsamples, fd);

	for (int i = 0; i < nsamples; i++)
	{
		*trace++ = *(data + i) / 32767.0f;
	}

#define readSection(section)                  \
	fread(sh, sizeof(section_header), 1, fd);   \
	n##section##s = sh->size / sizeof(section); \
	section##s = (section *)malloc(sh->size);   \
	fread(section##s, sizeof(section), sh->size / sizeof(section), fd);

	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(h2, sizeof(header2_t), 1, fd);
	//f("%.4s %u \n", h2->name, h2->size);

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
typedef struct _preset
{
	uint32_t pkey;
	rangesType vel, key;
	zone_t z;
	phdr hdr;
	struct _preset *next;
	struct _preset *link;
} preset;
preset *psets = NULL;
preset *isets = NULL;

preset *newPreset(phdr phr, pgen_t *g1, pgen_t *gl)
{
	preset *t = (preset *)malloc(sizeof(preset));
	t->hdr = phr;
	short *genset = (short *)malloc(sizeof(short) * 60);
	t->vel.lo = 0;
	t->vel.hi = 128;
	t->key.lo = 0;
	t->key.hi = 128;
	while (g1 != gl)
	{
		if (g1->operator== 43)
			t->key = g1->val.ranges;
		if (g1->operator== 44)
			t->vel = g1->val.ranges;
		genset[g1->operator] = g1->val.shAmount;
		g1++;
	}
	t->z = *(zone_t *)genset;
	t->next = NULL;
	t->link = NULL;
	return t;
}
int insert(preset **head, preset *newp, int pkey)
{
	preset **tr = head;
	while (*tr && &(*tr)->pkey < pkey)
		tr = &(*tr)->next;
	if (&(*tr)->pkey == pkey)
		return 0;

	newp->next = *tr;
	*tr = newp;
	return 1;
}
preset *IndexPresets()
{
	for (int i = 0; i < nphdrs - 1; i++)
	{
		int j = phdrs[i].pbagNdx;
#define pbg(j) pgens + (pbags + j)->pgen_id
		preset *t = newPreset(phdrs[i], pbg(j), pbg(j + 1));
		insert(&psets, t, phdrs[i].pid + phdrs[i].bankId);
		while (++j < phdrs[i + 1].pbagNdx)
		{
			preset *pbagPreset = newPreset(phdrs[i], pbg(j), pbg(j + 1));
			insert(&t->link, pbagPreset);
			if (pbagPreset->z.Instrument != -1)
			{
				int k = pbagPreset->z.Instrument;
				int l = insts[k].ibagNdx;
#define ibg(l) igens + (ibags + l)->igen_id
				preset *instDefault = newPreset(phdrs[i], ibg(l), ibg(l + 1));
				insert(&isets, instDefault);
				while (++l < insts[k + 1].ibagNdx)
				{
					preset *instPreset = newPreset(phdrs[i], ibg(l), ibg(l + 1));
					insert(&instDefault->link, instPreset);
				}
			}
		}
	}
	return psets;
}
#define trval (*tr)
#define trshift &((*tr)->next)
#define trspawn &((*tr)->link)

#define filterkv(trval, key, vel) \
	trval->key.lo > key || trval->key.hi<key || trval->vel.lo> vel || trval->vel.hi < vel

//depth-first prirnt
void dfprint(preset *p, int dep)
{
	preset **tr = &p;
	while (*tr)
	{
		int depc = dep;
		printf("\n");
		while (depc-- > 0)
			printf("--");
		printf("%d %s %d %d", trval->hdr.pid, trval->hdr.name, trval->key.lo, trval->key.hi);
		dfprint(p->link, dep + 1);
		tr = trshift;
	}
}

zone_t *get_sf(int pid, int bkid, int key, int vel)

{
	zone_t *z;
	short *attributes;
	short igset[60] = {0};
	int instID = -1;
	int lastSampId = -1;
	short pgdef[60] = {0};
	for (int i = 0; i < nphdrs - 1; i++)
	{
		if (phdrs[i].bankId != bkid || phdrs[i].pid != pid)
			continue;
		//	//f("\n%s %d", phdrs[i].name, key);
		int lastbag = phdrs[i + 1].pbagNdx;
		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;
			short pgset[60] = {0};
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
							if (igset[i] != 0)
								*(attributes + i) = igset[i];
							else if (igdef[i] != -1)
								*(attributes + 0) = igdef[i];
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
					else
					{
						memcpy(igdef, igset, sizeof(igdef)); // igdef = igset;
					}
				}
			}
		}
	}
	//return head;
	return z;
}