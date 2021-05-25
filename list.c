#ifndef NULL
#define NULL 0
#endif

typedef struct Node
{
	void *data;
	struct Node *next;
} node;

static node *front = NULL;
static node *rear = NULL;

void insert(node *front, void *data)
{
	node newNode;
	newNode.data = data;
	newNode.next = NULL;
	if (front == NULL)
		front = rear = &newNode;
	else
	{
		rear->next = &newNode;
		rear = &newNode;
	}
}