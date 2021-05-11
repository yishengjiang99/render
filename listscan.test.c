
#include "read2.c"
#include <printf.h>
void cb(void *data)
{
	printf("%f\n ", *(float *)data);
}
int main()
{
	//	read

	node *start = 0; //NULL;
	float f, d, b;
	f = 1, d = 2, b = 3;
	insert_node(&start, newNode(&b, 3));
	insert_node(&start, newNode(&d, 2));

	insert_node(&start, newNode(&f, 1));
	insert_node(&start, newNode(&d, 2));
	insert_node(&start, newNode(&b, 3));
	printNode(&start, &cb);
}