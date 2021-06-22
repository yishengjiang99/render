#include <stdio.h>

#include "sf2/sf2.h"  //.c"
#include "stdlib.h"
char *filename;
FILE *sf2, *output;

int main(int argc, char **argv) {
  filename = argc > 2 ? argv[2] : "GeneralUserGS.sf2";
  sf2 = fopen(filename, "rb");
  char outputname[1222];
  sprintf(outputname, "pages/%s.dat", filename);
  output = fopen(outputname, "wb");
  if (!output || !sf2) {
    perror("failed to open output or sf2 file");
    return 1;
  }
  sf2Info(sf2);
  for (int i = 0; i < 128; i++) {
    PresetZones pz = findByPid(i, 0);
    fwrite(&pz.hdr, sizeof(phdr), 1, output);
    fwrite(&pz.npresets, sizeof(int), 1, output);

    zone_t *zones = pz.zones;
    for (int z = 0; z < pz.npresets; z++) {
      shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);
      fwrite(sampl, sizeof(shdrcast), 1, output);
      fwrite(zones, sizeof(zones), pz.npresets, output);
    }
  }
  return 1;
}