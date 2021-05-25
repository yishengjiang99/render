#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include "call_ffp.c"
#define chrome "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome --headless \
--disable-gpu --dump-dom localhost"
int main()
{

	FILE *cc = popen(chrome, "r+");
	// while (1)
	// {
	// 	char c = fgetc(cc);
	// 	if (c == 0x00)
	// 		break;
	// 	fputc(c, stdout);
	// }

	//FILE *pipe_fp, *infile, *callserver, *send_ffplay;
	//	callserver = popen("nc localhost 50474 |ffplay -f f32le -i pipe:0", "w");

	// fputc(192, callserver);
	// fputc(0, callserver);
	// fputc(0x5f, callserver);
}