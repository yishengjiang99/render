#include <stdio.h>
#include <stdlib.h>
#include "call_ffp.c"
int main()
{
	FILE *pipe_fp, *infile, *callserver, *send_ffplay;
	callserver = popen("nc localhost 50474 |ffplay -f f32le -i pipe:0", "w");

	fputc(192, callserver);
	fputc(0, callserver);
	fputc(0x5f, callserver);
}