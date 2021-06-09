#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "gen_ops.h"
#include "sf2.c"
#define echo(str) fprintf(output, str);
static FILE *output;
void printpreset(char *file, int pid, int bankId) {
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
    short *attrs = (short *)zones;
    fprintf(output, "<tr>");
    fprintf(output, "<td>%hu-%hu</td>", zones->VelRange.lo, zones->VelRange.hi);
    fprintf(output, "<td>%hu-%hu</td>", zones->KeyRange.lo, zones->KeyRange.hi);

    fprintf(output,
            "<td><a href='#' class='pcm' sr='%d' file='%s' "
            "range='bytes=%d-%d' endloop='%d' startloop='%d'>sample (%d "
            "smpls)</a></td>",
            sampl->sampleRate, file, sdtastart + 2 * sampl->start,
            sdtastart + 2 * sampl->end + 1, sampl->endloop - sampl->start,
            sampl->startloop - sampl->start, sampl->end - sampl->start);

    fprintf(output, "<td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td>",
            zones->Attenuation, zones->FilterFc, zones->OverrideRootKey,
            zones->CoarseTune, zones->FineTune);
    char attrstr[60 * 10];
    for (int i = 0; i < 60; i++) {
      sprintf(attrstr + i * 10, "%hd,", attrs[i]);
    }
    fprintf(output, "<td><a href='#' class='attlist' attrs='");
    for (int i = 0; i < 60; i++) {
      fprintf(output, "%hd%s", attrs[i], i < 59 ? "," : "");
    }

    fprintf(output, "'>attrs</a>");

    fprintf(output, "</tr>");
    zones++;
  }
  fprintf(output, "  </table></details>");
}

int main(int argc, char **argv) {
  char *file = argc > 2 ? argv[2] : "GeneralUserGS.sf2";
  char outputfile[1024];
  sprintf(outputfile, "%s.html", file);
  output = fopen("index.html", "w+");
  if (!output) {
    perror("failed to open output file");
    return 1;
  }
  echo("<!DOCTYPE HTML><html>");
  echo("<head><style>font{size:14px}</style></head><body>");
  echo("<pre>");
  readsf(fopen(file, "rb"));
  echo("</pre>");
  fprintf(output, "<table><tr><td>");
  fprintf(output, "<main>");
  printpreset(file, 0, 0);

  for (int i = 0; i < 128; i++) {
    printpreset(file, i, 0);
  }
  for (int i = 0; i < 128; i++) {
    printpreset(file, i, 128);
  }
  echo("</td><td valign=top>");
  echo("<div id=details></div></td></tr></table>");
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