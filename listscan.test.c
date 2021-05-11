
#include "sf2.c"
#include <printf.h>
void cb(voice *v)
{
	printf("%f\n", v->ampvol->db_attenuate);
}
int main()
{
	//	read
	readsf(fopen("file.sf2", "rb"));
	node *start = 0; //NULL;
	short attr[60] = {-1};

	zone_t *z = (zone_t *)(attr);
	z->SampleId = 55;
	z->VolEnvAttack = -1200;
	insert_node(&start, newNode(newVoice(z, 55, 33), 5));	 // //; //33, 66), 2));
	insert_node(&start, newNode(newVoice(z, 56, 33), 35)); // //; //33, 66), 2));
	insert_node(&start, newNode(newVoice(z, 56, 33), 33)); // //; //33, 66), 2));

	printNode(&start, &cb);

	remove_node(&start, 5);
	printNode(&start, &cb);
	remove_node(&start, 35);
	printNode(&start, &cb);
}