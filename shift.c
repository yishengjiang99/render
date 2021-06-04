
void loop(float *sample, float *output, float *ratio, int offset, float fract)
{

	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sample + offset);
		float f2 = *(sample + offset + 1);
		float o1 = *(output + 2 * i + 1);
		float o2 = *(output + 2 * i);
		float gain = f1 + (f2 - f1) * fract;

		float mono = gain * centdblut(envShift(v->ampvol)); // + attentuate); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		*(output + 2 * i) += mono * v->panRight;
		*(output + 2 * i + 1) += mono * v->panLeft;

		v->frac += v->ratio;
		while (v->frac >= 1.0f)
		{
			v->frac--;
			v->pos++;
		}
		while (v->pos >= v->endloop)
		{
			v->pos = v->pos - loopLength + 1;
		}
	}
}
