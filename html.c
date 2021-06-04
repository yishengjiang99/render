#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sf2.c"
#include "runtime.c"
void echo(char *str, ...)
{
	va_list args;

	fprintf(stdout, str, args);
}
int main()
{

	readsf(fopen("GeneralUserGS.sf2", "rb"));

	init_ctx();
	setProgram(0, 0);
	for (int i = 0; i < g_ctx->channels[0].pzset.npresets; i++)
	{
		printf("\n%hd %f", g_ctx->channels[0].pzset.zones[i].FilterF, g_ctx->channels[0].pzset.zones[i].FilterFc, powf(2, (float)g_ctx->channels[0].pzset.zones[i].FilterFc / 1200.0f));
	}
}