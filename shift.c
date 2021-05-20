

void loop(voice *v, float *output, int flag)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = v->z->Attenuation;
	short pan = v->z->Pan;
	float panLeft = sqrtf(0.5f - pan / 1000.0f);
	float panright = sqrtf(0.5f + pan / 1000.0f);
	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float o1 = *(output + 2 * i + 1);
		float o2 = *(output + 2 * i);
		float gain = f1 + (f2 - f1) * v->frac;

		float mono = gain * centdblut(envShift(v->ampvol) + (int)attentuate); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		if (flag == 1)
		{
			*(output + 2 * i) = o1 * 0.8f + mono * panright;
			*(output + 2 * i + 1) = o2 * 0.8f + mono * panLeft;
		}
		else if (flag > 5)
		{

			*(output + 2 * i) = o1 * 0.8f + mono * panright / (float)flag;
			*(output + 2 * i + 1) = o2 * 0.8f + mono * panLeft / (float)flag;
		}
		else
		{
			*(output + 2 * i) += mono * panright * 0.8;
			*(output + 2 * i + 1) += mono * panLeft * 0.8;
		}

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
