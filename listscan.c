#ifndef malloc
#include <mm_malloc.h>
#endif
#ifndef voice_h
#include "voice.c"
#endif
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

node *newNode(voice *v, int index)
{
	node *nd = (node *)malloc(sizeof(node));
	nd->voice = v;
	nd->index = index;
	nd->next = 0;
	return nd;
}
void printNode(node **start, void (*cb)(void *voice)) //(void) cb(void *data))
{
	node **tracer = start;

	while (*tracer)
	{
		cb((*tracer)->voice);
		tracer = &(*tracer)->next;
	}
}
