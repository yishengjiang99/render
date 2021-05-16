

void loop(voice *v, float *output)
{
	uint32_t loopLength = v->endloop - v->startloop;
	float attentuate = v->z->Attenuation + velCB[v->velocity];
	short pan = v->z->Pan;

	float panLeft = sqrtf(0.5f - pan / 1000.0f);
	float panright = sqrtf(0.5f + pan / 1000.0f);
	for (int i = 0; i < 128; i++)
	{
		float f1 = *(sdta + v->pos);
		float f2 = *(sdta + v->pos + 1);
		float gain = f1 + (f2 - f1) * v->frac;

		float mono = gain * centdblut(envShift(v->ampvol) + (int)attentuate); //[(int)envShift(v->ampvol)]; //* centdbLUT[v->z->Attenuation];
		*(output + 2 * i) += mono * panright;
		*(output + 2 * i + 1) += mono * panLeft;
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
void loopctx(voice *v, ctx_t *ctx)
{
	loop(v, ctx->outputbuffer);
}