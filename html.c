#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "gen_ops.h"
#include "sf2.c"
#define echo(str) fprintf(output, str);
static FILE *output;
void printpreset(char *file, int pid, int bankId) {
  char misc[1024];
  uint32_t miscoffset = 0;
  PresetZones pz = findByPid(pid, bankId);

  if (pz.npresets == 0) return;
  fprintf(output,
          "\n<details style='overflow-x:auto;'> <summary><span>%s "
          "(%d)</span></summary>",
          pz.hdr.name, pz.hdr.pid);
  fprintf(output, " \n <table class='table-auto'>");

  zone_t *zones = pz.zones;
  for (int j = 0; j < pz.npresets - 1; j++) {
    shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);
    fprintf(output, "<tr>");
    fprintf(output, "<td>%hu-%hu</td>", zones->VelRange.lo, zones->VelRange.hi);
    fprintf(output, "<td>%hu-%hu</td>", zones->KeyRange.lo, zones->KeyRange.hi);
    fprintf(output,
            "<td>%d <a href='render.php?range=%d-%d'>Play sample</a></td>",
            sampl->end - sampl->start, sampl->start, sampl->end);
    fprintf(
        output,
        "<td><a href='#'class='pcm' file='%s' range='bytes=%d-%d'>sample (%d "
        "smpls)</a></td>",
        file, sdtastart + 2 * sampl->start, sdtastart + 2 * sampl->end + 1,
        sampl->end - sampl->start);
    fprintf(output, "<td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td>",
            zones->Attenuation, zones->FilterFc, zones->OverrideRootKey,
            zones->CoarseTune, zones->FineTune);
    fprintf(output, "</tr>");
    zones++;
  }
  fprintf(output, "  </table></details>");
}

int main(int argc, char **argv) {
  char *file = argc > 2 ? argv[2] : "GeneralUserGS.sf2";
  char outputfile[1024];
  sprintf(outputfile, "%s.html", file);
  output = fopen(outputfile, "w+");
  if (!output) {
    perror("failed to open output file");
    return 1;
  }
  echo("<!DOCTYPE HTML><html>");
  echo("<head></head><body>");
  echo("<pre>");
  readsf(fopen(file, "rb"));
  echo("</pre>");

  fprintf(output, "<main>");
  printpreset(file, 0, 0);

  // for (int i = 0; i < 128; i++) {
  //   printpreset(file, i, 0);
  // }
  // for (int i = 0; i < 128; i++) {
  //   printpreset(file, i, 128);
  // }
  echo("</main>");
  echo("<script>");
  FILE *js = fopen("playsample.js", "r");
  char c;
  while (!feof(js)) {
    char c = fgetc(js);
    if (c > 0x00) fputc(c, output);
  }
  echo("</script>");

  echo("</body></html>");
}