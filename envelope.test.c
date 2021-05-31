
const float sampleRate = 44100;
float coeff;
float currentLevel;
#include <math.h>
#include <stdio.h>
void init(float levelBegin, float levelEnd, float releaseTime)
{
	currentLevel = levelBegin;
	coeff = (log(levelEnd) - log(levelBegin)) /
					(releaseTime * sampleRate);
}

void calculateEnvelope(int samplePoints)
{
	for (int i = 0; i < samplePoints; i++)
	{
		currentLevel += coeff * currentLevel;
	}
}

int main()
{
	init(.899, .333, 1.0f);
	calculateEnvelope(48000);
}