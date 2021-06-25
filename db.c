#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "sf2/sf2.c"
static inline int file_get_contents(FILE *fd, char *ptr) {
  fseek(fd, 0, SEEK_END);
  long size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  ptr = malloc(size);
  fread(ptr, 1, size, fd);
  return size;
}
void writeRiff(char *readff, char *outputff);
void readRiff(char *data, int len);
int main(int argc, char **argv) {
  char *readff = argc > 2 ? argv[2] : "file.sf2";

  char *outputff = argc > 3 ? argv[3] : "file.riff";
  writeRiff(readff, outputff);
  char *data;
  FILE *fd = fopen(outputff, "rb");
  fseek(fd, 0, SEEK_END);
  long size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  char *ptr = malloc(size);
  fread(ptr, 1, size, fd);

  readRiff(ptr, size);
}

void writeRiff(char *readff, char *outputff) {
  readsf(readff);
  FILE *output = fopen(outputff, "w");
  for (int i = 0; i < 128; i++) {
    PresetZones pz = findByPid(i, 0);
    if (pz.npresets == 0) continue;
    fwrite(&(pz.hdr), sizeof(phdr), 1, output);
    fwrite(&(pz.npresets), sizeof(int), 1, output);

    zone_t *zone = pz.zones;
    zone_t *last = pz.zones + pz.npresets;
    while (zone != last) {
      shdrcast *sampl = (shdrcast *)(shdrs + zone->SampleId);
      fwrite(sampl, sizeof(shdrcast), 1, output);
      fwrite(zone, sizeof(zone_t), 1, output);
      zone++;
    }
  }
}

void readRiff(char *ptr, int size) {
  char *last = ptr + size;
  int npresets;
  shdrcast *smpl;
  zone_t *z;
  while (ptr < last) {
    phdr *pz = (phdr *)ptr;
    ptr += sizeof(phdr);
    int npresets = *(int *)ptr;
    ptr += sizeof(int);
    ptr += npresets * sizeof(zone_t) + npresets * sizeof(shdrcast);
    printf("%s \n %d \n", pz->name, npresets);
    //     while (npresets-- >= 0) {
    //       smpl = (shdrcast *)ptr;
    //       ptr += sizeof(shdrcast);
    //       z = (zone_t *)ptr;
    //       ptr += sizeof(zone_t);
    //     }
  }
}