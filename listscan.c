#ifndef malloc
#include <mm_malloc.h>
#endif
#ifndef voice_h
#include "voice.c"
#endif
#define TRUE 1
#define FALSE 0
typedef int BOOL;

typedef struct _node
{
	voice *voice;
	int index;
	struct _node *next;
} node;

void insert_node(node **head, node *newp)
{
	node **tracer = head;

	while ((*tracer) && (*tracer)->index < newp->index <= 0)
	{
		tracer = &(*tracer)->next;
	}

	newp->next = *tracer;
	*tracer = newp;
}
node *remove_node(node **head, int index)
{
	BOOL present = FALSE;
	node *rem;
	node **tracer = head;

	while ((*tracer) && !(present = (index == (*tracer)->index)))
		tracer = &(*tracer)->next;
	if (present)
	{
		rem = *tracer;
		*tracer = (*tracer)->next;
		return rem;
	}
	return NULL;
}

node *newNode(voice *v, int index)
{
	node *nd = (node *)malloc(sizeof(node));
	nd->voice = v;
	nd->index = index;
	nd->next = 0;
	return nd;
}
void printNode(node **start, void (*cb)(voice *voice)) //(void) cb(void *data))
{
	node **tracer = start;

	while (*tracer)
	{
		cb((*tracer)->voice);
		tracer = &(*tracer)->next;
	}
}
