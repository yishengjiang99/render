#include "sf2.c"
#include "list.c"

typedef struct zone
{
	int pid;
	int lowKey, hiKey;
	short attr[60];
	zone_t *next;
} zone_t;

zone_t *head = NULL;
zone_t *newZone()
{
	zone_t *nz;
	nz = (zone_t *)malloc(sizeof(zone_t));
	nz->next = NULL;
}
void insert(zone_t *z)
{
}
int main()
{
	readsf(fopen("file.sf2", "rb"));
	zone_t **head = NULL;
	for (int i = 0; i < nphdrs - 1; i++)
	{
		zone_t *z = newZone();
		z->pid = phdrs[i].pid | phdrs[i].bankId;
	}

	int pg = pbags[phdrs[0].pbagNdx].pgen_id;
	pgen_t *pg = (pgen_t)
}