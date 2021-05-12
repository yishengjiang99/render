#include <stdio.h>
#include <math.h>
static float p2over1200LUT[1200];
static inline float p2over1200(float x)
{
	if (x < -12000)
		return 0;
	if (x < 0)
		return 1.f / p2over1200(-x);
	else if (x > 1200.0f)
	{
		return 2 * p2over1200(x - 1200.0f);
	}
	else
	{
		return p2over1200LUT[(unsigned short)(x)];
	}
}
static float centdbLUT[960];
static float centdblut(int x)
{
	if (x < 0)
		x = 0;
	if (x > 960)
		x = 960;

	return centdbLUT[x];
}
void initLUTs()
{
	FILE *f = fopen("luts.c", "w");
	fprintf(f, "#define LUT 1\n float p2over1200LUT[1200] = {");
	for (int i = 0; i < 1199; i++)
	{
		p2over1200LUT[i] = powf(2.0f, i / 1200.0f);
		fprintf(f, "%ff,", powf(2.0f, i / 1200.0f));
	}
	fprintf(f, "2.0f};\n");
	fprintf(f, "float centdbLUT[1440] = {");
	for (int i = 0; i < 1439; i++)
	{
		centdbLUT[i] = powf(10.0f, i / -200.0);
		fprintf(f, "%ff,", powf(10.0f, i / -200.0));
	}
	fprintf(f, "0.0f};");
}

int main()
{
	initLUTs();
}
