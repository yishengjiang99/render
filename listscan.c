#define listscan 1
#ifndef listscan
#include "listscan.c"

#endif
#ifndef malloc
#include <mm_malloc.h>
#endif

#define TRUE 1
#define FALSE 0
typedef int BOOL;

typedef struct _node
{
	void *voice;

	uint32_t index;
	struct _node *next;
} node;

void insert_node(node **head, node *newp)
{
	newp->next = *head;
	*head = newp;
}
node *find_node(node **head, int index)
{
	BOOL present = FALSE;
	node **tracer = head;

	while ((*tracer) && !(present = (index == (*tracer)->index)))
		;
	if (present)
	{
		return *tracer;
	}
	else
	{
		return NULL;
	}
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

node *newNode(void *v, int index)
{
	node *nd = (node *)malloc(sizeof(node));
	nd->voice = v;
	nd->index = index;
	nd->next = 0;
	return nd;
}
void iterate(void *opaque, node **start, void (*cb)(void *opaque, void *voice))
{
	node **tracer = start;
	while ((*tracer) != NULL && (*tracer)->voice)
	{
		cb(opaque, (*tracer)->voice);
		tracer = &(*tracer)->next;
	}
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
