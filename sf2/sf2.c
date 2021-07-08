#ifndef SF2_C
#define SF2_C

#include "../includes/sf2.h"

#include <stdlib.h>
#include <string.h>

#include "sf2zone.c"
#include <stdio.h>

void read_sdta(FILE *fd) {
  if (sdtastart) {
    fseek(fd, sdtastart, SEEK_SET);
    data = (short *)malloc(nsamples * sizeof(short));
    sdta = (float *)malloc(nsamples * sizeof(float));
    float *trace = sdta;

    fread(data, sizeof(short), nsamples, fd);
    for (int i = 0; i < nsamples; i++) {
      *trace++ = *(data + i) / 32767.0f;
    }
  }
}
void sf2Info(FILE *fd) {
  header2_t *rrff = (header2_t *)malloc(sizeof(rrff));
  fseek(fd,8,SEEK_CUR);
    fread(rrff, sizeof(header2_t), 1, fd);

//  header2_t *rrff = (header2_t *)malloc(sizeof(header2_t));
  fread(rrff, sizeof(rrff), 1, fd);

  info =(char*) malloc(rrff->size);
  for (int i = 0; i < rrff->size; i++)
    info[i]=(fgetc(fd) & 0x7f); //reall don't feel like parsing this so just asci-masking the readable str.

  fread(rrff, sizeof(header2_t), 1, fd);
  nsamples = rrff->size / sizeof(short);
  sdtastart = ftell(fd);

  fseek(fd, rrff->size, SEEK_CUR);

  // fread(data, sizeof(short), nsamples, fd);

  // for (int i = 0; i < nsamples; i++) {
  //   *trace++ = *(data + i) / 32767.0f;
  // }

#define readSection(section)                  \
  fread(sh, sizeof(section_header), 1, fd);   \
  n##section##s = sh->size / sizeof(section); \
  section##s = (section *)malloc(sh->size);   \
  fread(section##s, sizeof(section), sh->size / sizeof(section), fd);

  section_header *sh = (section_header *)malloc(sizeof(section_header));

  fread(rrff, sizeof(header2_t), 1, fd);
  readSection(phdr);

  readSection(pbag);
  readSection(pmod);
  readSection(pgen);
  readSection(inst);
  readSection(ibag);
  readSection(imod);
  readSection(igen);
  readSection(shdr);
  presetZones = (PresetZones *)malloc(nphdrs * sizeof(PresetZones));
  for (int i = 0; i < nphdrs; i++) {
    *(presetZones + i) = findPresetZones(i, findPresetZonesCount(i));
  }
}

void readsf(char *filename) {
  FILE *fd = fopen(filename, "rb");
  sf2Info(fd);
  read_sdta(fd);
}

void read_sf2_mem(void *mem, int n) {
  FILE *fd = fmemopen(mem, n, "rb");
  sf2Info(fd);
  read_sdta(fd);
}
#endif