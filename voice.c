#ifndef read2_h
#include "sf2.c"
#endif
#ifndef adsr
#include "envelope.c"
#endif
#define voice_h 1
typedef struct _voice
{
	zone_t *z;
	shdrcast *sample;
	unsigned int start, end, startloop, endloop;
	uint32_t pos;
	float frac;
	float ratio;
	adsr_t *ampvol;
	int midi;
	int velocity;
	int chid;
} voice;

voice *newVoice(zone_t *z, int midi, int vel)
{
	shdrcast *sh = (shdrcast *)(shdrs + z->SampleId);
	voice *v = (voice *)malloc(sizeof(voice));
	v->sample = sh;
	v->start = sh->start + ((unsigned short)(z->StartAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->StartAddrOfs & 0x7f);
	v->end = sh->end + ((unsigned short)(z->EndAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndAddrOfs & 0x7f);
	v->endloop = sh->endloop + ((unsigned short)(z->EndLoopAddrCoarseOfs & 0x7f) << 15) + (unsigned short)(z->EndLoopAddrOfs & 0x7f);
	v->startloop = sh->startloop + (unsigned short)(z->StartLoopAddrCoarseOfs & 0x7f << 15) + (unsigned short)(z->StartLoopAddrOfs & 0x7f);
	v->ampvol = newEnvelope(z->VolEnvAttack, z->VolEnvRelease, z->VolEnvDecay, z->VolEnvSustain, 48000);
	short rt = z->OverrideRootKey > -1 ? z->OverrideRootKey : sh->originalPitch;
	float sampleTone = rt * 100.0f + z->CoarseTune * 100.0f + (float)z->FineTune;
	float octaveDivv = ((float)midi * 100 - sampleTone) / 1200.0f;
	v->ratio = 1.0f * pow(2.0f, octaveDivv) * sh->sampleRate / 48000;

	v->pos = v->start;
	v->frac = 0.0f;
	v->z = z;
	v->midi = midi;
	v->velocity = vel;
	return v;
}