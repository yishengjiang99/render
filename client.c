#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include "call_ffp.c"
#define chrome "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome --as-browser --remote-debugging-port=9222 https://grepawk.com"
int main()
{

	FILE *c = popen(chrome, "r+");

	//FILE *pipe_fp, *infile, *callserver, *send_ffplay;
	//	callserver = popen("nc localhost 50474 |ffplay -f f32le -i pipe:0", "w");

	// fputc(192, callserver);
	// fputc(0, callserver);
	// fputc(0x5f, callserver);
}