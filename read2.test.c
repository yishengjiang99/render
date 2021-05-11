#include "read2.c"

void iter_callback(void *data)
{
	voice *v = (voice *)data;
	zone_t *z = v->z;

	printf("\n %d %d %d  %d", (unsigned short)z->SampleId, (short)z->Attenuation, (short)z->ReverbSend, (unsigned char)v->midi);
}
int main(int argc, char const *argv[])
{
	readsf(fopen("file.sf2", "rb"));
	node *start = 0;
	get_sf(0, 0, 55, 33, &start);
	get_sf(0, 0, 33, 22, &start);
	printNode(&start, &iter_callback);

	/* code */
	return 0;
}
