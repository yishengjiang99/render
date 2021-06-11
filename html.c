#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/fft.c"
#include "runtime.c"
#include "sf2.c"
#include "stbl.c"
#define echo(str) fprintf(output, str);
static FILE *output;
void printpreset(char *file, int pid, int bankId) {
  PresetZones pz = findByPid(pid, bankId);
  complex c[4096];
  complex *it = c;

  if (pz.npresets == 0) return;
  fprintf(output,
          "\n<details style='overflow-x:auto;'> <summary><span>%s "
          "(%d)</span></summary>",
          pz.hdr.name, pz.hdr.pid);
  fprintf(output, " \n <table border=1>");
  init_ctx();

  zone_t *zones = pz.zones;
  for (int j = 0; j < pz.npresets - 1; j++) {
    shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);
    short *attrs = (short *)zones;
    voice *v = newVoice(zones, zones->KeyRange.lo, zones->KeyRange.hi, 0);
    loop(v, g_ctx->outputbuffer, g_ctx->channels[0]);
    bzero(c, 4096 * sizeof(complex));
    for (int k = 0; k < 4096; k++) {
      c[k].real = g_ctx->outputbuffer[k];
      c[k].imag = 0.0f;
    }
    FFT(c, 12, stbl);
    bit_reverse(c, 12);

    fprintf(output, "<tr>");
    fprintf(output, "<td><a href='#' class='fftbins' real='");
    for (int i = 0; i < 4096; i++) {
      fprintf(output, "%f%s", c[i].real, i < 4095 ? "," : "");
    }
    fprintf(output, "' img='");
    for (int i = 0; i < 4096; i++) {
      fprintf(output, "%f%s", c[i].imag, i < 4095 ? "," : "");
    }
    fprintf(output, "'>fftbins</a>");

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
  for (int i = 0; i < 55; i++) {
    printpreset(file, phdrs[i].pid, phdrs[i].bankId);
  }
  echo("</td><td valign=top>");
  echo("<canvas></canvas><div id=details></div></td><td></td></tr></table>");
  echo(" <script type='module'> import {chart} from './chart.js'; ");

  FILE *js = fopen("playsample.js", "r");
  char c;
  while (!feof(js)) {
    char c = fgetc(js);
    if (c > 0x00) fputc(c, output);
  }
  echo("</script>");

  echo("</body></html>");
}