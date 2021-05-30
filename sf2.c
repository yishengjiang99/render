#ifndef SF2_C
#define SF2_C

#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <string.h>
#include "sf2.h"

int readsf(FILE *fd)
{
	sheader_t *header = (sheader_t *)malloc(sizeof(sheader_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(sheader_t), 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	info = malloc(h2->size);
	fread(info, h2->size, 1, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	data = (short *)malloc(h2->size / 2 * sizeof(short));
	sdta = (float *)malloc(h2->size / 2 * sizeof(float));
	float *trace = sdta;
	nsamples = h2->size / sizeof(short);

	printf("\n\t %ld", ftell(fd));
	fread(data, sizeof(short), nsamples, fd);

	for (int i = 0; i < nsamples; i++)
	{
		*trace++ = *(data + i) / 32767.0f;
	}

#define readSection(section)                  \
	fread(sh, sizeof(section_header), 1, fd);   \
	printf("%.4s:%u\n", sh->name, sh->size);    \
	n##section##s = sh->size / sizeof(section); \
	section##s = (section *)malloc(sh->size);   \
	fread(section##s, sizeof(section), sh->size / sizeof(section), fd);

	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(h2, sizeof(header2_t), 1, fd);
	printf("%.4s %u \n", h2->name, h2->size);

	readSection(phdr);

	readSection(pbag);
	readSection(pmod);
	readSection(pgen);
	readSection(inst);
	readSection(ibag);
	readSection(imod);
	readSection(igen);
	readSection(shdr);
	return 1;
}
#endif