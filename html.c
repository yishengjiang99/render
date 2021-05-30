#include <stdlib.h>
#include <stdio.h>
int main()
{
	FILE *f = popen("nc -L 4000", "w");
}