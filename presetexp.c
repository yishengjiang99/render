#ifndef PRESETEXP_C
#define PRESETEXP_C

#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <string.h>
#include "sf2.c"
#define trval (*tr)
#define trshift &((*tr)->next)
#define trspawn &((*tr)->link)
#define pbg(j) pgens + (pbags + j)->pgen_id
#define ibg(l) igens + (ibags + l)->igen_id
#define filterkv(trval, key, vel) \
	(trval->key).lo > key || (trval->key).hi<key || trval->vel.lo> vel || trval->vel.hi < vel

#define merge(attra, attrb)    \
	for (int i = 0; i < 60; i++) \
		attra[i] = attrb[i];

#define mergeadd(attra, attrb) \
	for (int i = 0; i < 60; i++) \
		attra[i] += attrb[i];      \
	retrun attra;
#define sr 48000

#define _GEN(_name) _name
/* See SFSpec21 $8.1.3 */
typedef struct
{
	int idc, idc2;
	float min, max, defaultv;
} defaults_t;
static const defaults_t defaults[] = {
		{1, 1, 0.0f, 1e10f, 0.0f},
		{1, 1, -1e10f, 0.0f, 0.0f},
		{1, 1, -1e10f, 1e10f, 0.0f},
		{1, 1, -1e10f, 1e10f, 0.0f},
		{0, 1, 0.0f, 1e10f, 0.0f},
		{1, 2, -12000.0f, 12000.0f, 0.0f},
		{1, 2, -12000.0f, 12000.0f, 0.0f},
		{1, 2, -12000.0f, 12000.0f, 0.0f},
		{1, 2, 1500.0f, 13500.0f, 13500.0f},
		{1, 1, 0.0f, 960.0f, 0.0f},
		{1, 2, -12000.0f, 12000.0f, 0.0f},
		{1, 2, -12000.0f, 12000.0f, 0.0f},
		{0, 1, -1e10f, 0.0f, 0.0f},
		{1, 1, -960.0f, 960.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{1, 1, 0.0f, 1000.0f, 0.0f},
		{1, 1, 0.0f, 1000.0f, 0.0f},
		{1, 1, -500.0f, 500.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 4, -16000.0f, 4500.0f, 0.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 4, -16000.0f, 4500.0f, 0.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{0, 1, 0.0f, 1000.0f, 0.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{0, 1, -1200.0f, 1200.0f, 0.0f},
		{0, 1, -1200.0f, 1200.0f, 0.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{1, 2, -12000.0f, 5000.0f, -12000.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{0, 1, 0.0f, 1440.0f, 0.0f},
		{1, 2, -12000.0f, 8000.0f, -12000.0f},
		{0, 1, -1200.0f, 1200.0f, 0.0f},
		{0, 1, -1200.0f, 1200.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 127.0f, 0.0f},
		{0, 0, 0.0f, 127.0f, 0.0f},
		{0, 1, -1e10f, 1e10f, 0.0f},
		{1, 0, 0.0f, 127.0f, -1.0f},
		{1, 1, 0.0f, 127.0f, -1.0f},
		{1, 1, 0.0f, 1440.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 1, -1e10f, 1e10f, 0.0f},
		{0, 1, -120.0f, 120.0f, 0.0f},
		{0, 1, -99.0f, 99.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{0, 1, 0.0f, 1200.0f, 100.0f},
		{0, 0, 0.0f, 0.0f, 0.0f},
		{1, 0, 0.0f, 127.0f, -1.0f},
		{1, 0, 0.0f, 127.0f, 0.0f},
		{1, 0, -960.0f, 960.0f, 0.0f},
		{1, 2, 0.0f, 22050.0f, 0.0f},
		{1, 1, 0.0f, 960.0f, 0.0f}};

typedef struct _preset
{

	struct _preset *next;
	struct _preset *link;
	int pkey;
	rangesType vel, key;
	short attrs[60];
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
	t->vel.lo = 0;
	t->vel.hi = 128;
	t->key.lo = 0;
	t->key.hi = 128;

	while (g1 != gl)
	{
		switch (g1->genid)
		{
		case VelRange:
			t->vel = g1->val.ranges;
			break;
		case KeyRange:
			t->key = g1->val.ranges;
			break;
		case StartAddrOfs:
		case EndAddrOfs:
		case StartLoopAddrOfs:
		case EndLoopAddrOfs:
		case StartAddrCoarseOfs:
		case EndAddrCoarseOfs:
		case EndLoopAddrCoarseOfs:
		case StartLoopAddrCoarseOfs:
			t->attrs[g1->genid] = g1->val.uAmount;
			break;
		default:
			t->attrs[g1->genid] = g1->val.shAmount;
			break;
		}
		g1++;
	}
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
		while (++j < llastpbag)
		{
			//s("\n \t pbag %d", j);
			preset *pbagPreset = newPreset(phdrs[i], pbg(j), pbg(j + 1));
			insert(&pdef->link, pbagPreset, pdef->pkey << 7 | j);
			if (pbagPreset->z.Instrument > -1)
			{
				short k = pbagPreset->z.Instrument;

				int l = insts[k].ibagNdx;
				preset *instDefault = newPreset(phdrs[i], ibg(l), ibg(l + 1));
				insert(&(pbagPreset->link), instDefault, pbagPreset->pkey << 7 | l);
				while (++l < insts[k + 1].ibagNdx)
				{
					preset *instPreset = newPreset(phdrs[i], ibg(l), ibg(l + 1));
					insert(&(pbagPreset->link), instPreset, l);
				}
			}
		}
	}
	return psets;
}
#define printz(trval)                               \
	printf("\n%s %d %d, \n\rsamp %d inst %d\n",       \
				 trval->hdr.name, trval->hdr.pid,           \
				 trval->hdr.bankId, trval->attrs[SampleId], \
				 trval->attrs[Instrument]);                 \
	// printf("%d %d %d %d\n", trval->key.lo,               \
	// 			 trval->key.hi, trval->vel.lo, trval->vel.hi); \
	for (int i = 0; i < 60; i++)                         \
		printf("%d,", trval->attrs[i]);

void dfprint(preset *p, int dep)
{
	if (dep > 3)
		return;
	preset **tr = &p;
	if (trval != NULL)
	{
		int depc = dep;
		printf("\n");
		while (depc-- > 0)
			printf("--");

		if (trval->link)
			dfprint(trspawn, dep + 1);
		tr = trshift;
		printz(trval);
		dfprint(trval, dep);
		//dfprint(trshift, dep);
	}
}

int main()
{
	readsf(fopen("file.sf2", "rb"));
	preset *p = IndexPresets();
	dfprint(p, 1);
}

// 	for (preset **tr = &pdef->link; trval != NULL; trshift)
// 	{
// 		short patts[60], atts[60];

// 		if (trval->key.hi < key && trval->key.lo > key)
// 		{
// 			//patts = merge(pdef->attrs, trval->attrs);

// 			tr = trspawn;
// 			preset *idef = trval;
// 			for (tr = trspawn; trval != NULL; trshift)
// 			{
// 				if (trval->key.hi < key && trval->key.lo > key)
// 				{
// 					merge(atts, idef->attrs);
// 					merge(atts, trval->attrs);
// 					//mergeAdd();
// 				}
// 			}
// 		}
// 	}
// 	printz(trval->link);
// 	tr = trspawn;
// 	do
// 	{
// 		printz(trval);
// 		tr = trshift;
// 	} while (*trshift);
// 	tr = trspawn;
// 	printf("ibgdef");
// 	printz(trval);

// 	while (trval != NULL)
// 	{
// 		printz(trval);

// 		tr = trshift;
// 	}

// 	//	dfprint(h, 0); ///p, 0);
// }
#endif