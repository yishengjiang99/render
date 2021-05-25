#ifndef PRESETEXP_C
#define PRESETEXP_C

#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <string.h>
#include "sf2.c"
#include "voice.c"
#define trval (*tr)
#define trshift &((*tr)->next)
#define trspawn &((*tr)->link)
#define pbg(j) pgens + (pbags + j)->pgen_id

#define filterkv(trval, key, vel) \
	(trval->key).lo > key || (trval->key).hi<key || trval->vel.lo> vel || trval->vel.hi < vel

#define sr 48000
typedef struct _preset
{

	struct _preset *next;
	struct _preset *link;
	int pkey;
	rangesType vel, key;
	zone_t z;
	phdr hdr;
} preset;

void printz(preset *p)
{
	printf("\n/* %s %d %d %d", p->hdr.name, p->hdr.pid, p->pkey, p->hdr.bankId);
	printf("\tKeyRange: %d %d", p->key.lo, p->key.hi);
}
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
		if (g1->genid == 43)
			t->key = g1->val.ranges;
		else if (g1->genid == 44)
		{
			t->vel = g1->val.ranges;
		}
		else
		{
			genset[g1->genid] = g1->val.shAmount;
		}
		g1++;
	}
	t->z = *((zone_t *)genset);
	t->next = NULL;
	t->link = NULL;
	return t;
}
int insert(preset **head, preset *newp, int pkey)
{
	preset **tr = head;
	while (*tr)
	{
		if (trval->pkey > pkey)
			break;
		else if (trval->pkey == pkey)
			return 0;
		else
			tr = trshift;
	}
	newp->pkey = pkey;

	newp->next = *tr;
	*tr = newp;
	return 1;
}

preset *ff(preset *p, uint16_t pid, uint16_t bid)
{
	while (p)
	{
		if (p->hdr.bankId == bid && p->hdr.pid == pid)
			return p;

		p = p->next;
	}

	return NULL;
}
preset *IndexPresets()
{
	preset *psets = NULL;
	for (unsigned short i = 0; i < nphdrs - 1; i++)
	{
		unsigned short j = phdrs[i].pbagNdx;
		preset *pdef = newPreset(phdrs[i], pbg(j), pbg(j + 1));
		unsigned short llastpbag = phdrs[i + 1].pbagNdx;
		insert(&psets, pdef, phdrs[i].pid + (phdrs[i].bankId << 7));
		//s("pdef %d \n", j);

		while (++j < llastpbag)
		{
			//s("\n \t pbag %d", j);
			preset *pbagPreset = newPreset(phdrs[i], pbg(j), pbg(j + 1));
			insert(&(pdef->link), pbagPreset, pdef->pkey << 7 | j);
			if (pbagPreset->z.Instrument > -1)
			{
				short k = pbagPreset->z.Instrument;

				int l = insts[k].ibagNdx;
#define ibg(l) igens + (ibags + l)->igen_id
				preset *instDefault = newPreset(phdrs[i], ibg(l), ibg(l + 1));
				insert(&(pbagPreset->link), instDefault, pbagPreset->pkey << 7 | l);

				while (++l < insts[k + 1].ibagNdx)
				{
					preset *instPreset = newPreset(phdrs[i], ibg(l), ibg(l + 1));
					insert(&instDefault, instPreset, l);
				}
			}
		}
	}
	return psets;
}

//depth-first prirnt
void dfprint(preset *p, int dep)
{
	preset **tr = &p;
	while (*tr)
	{
		int depc = dep;
		//s("\n");
		while (depc-- > 0)
			//s("--");
			//s("%d %s %d %d", trval->hdr.pid, trval->hdr.name, trval->key.lo, trval->key.hi);
			dfprint(p->link, dep + 1);
		tr = trshift;
	}
}
#endif