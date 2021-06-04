#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sf2.h"
void echo(char *str, ...)
{
	va_list args;

	fprintf(stdout, str, args);
}
int main()
{

	readsf(fopen("GeneralUserGS.sf2", "rb"));
	short attrs[60] = defattrs;
	zone_t *z = (zone_t *)z;
	printf("%d", z->OverrideRootKey);
	init_ctx();
	readsf(fopen("GeneralUserGS.sf2", "rb"));
	setProgram(9, 0);
	noteOn(9, 44, 55, 0);
}