#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libs/fft.h"
#include "runtime.h"
#include "sf2.h"
#include "stbl.c"

FILE *output, *sf2;
char *filename;
char template[1024 * 56];
char javascript[1024 * 56];  //*javascript;
int matched;
#define echo(str) fprintf(output, "%s", str);

static inline void file_get_contents(FILE *fd, char *ptr) {
  fseek(fd, 0, SEEK_END);
  long size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  fread(ptr, 1, size, fd);
}

int main(int argc, char **argv) {
  filename = argc > 2 ? argv[2] : "GeneralUserGS.sf2";
  sf2 = fopen(filename, "rb");
  output = fopen("staging.html", "wb");
  if (!output || !sf2) {
    perror("failed to open output or sf2 file");
    return 1;
  }
  file_get_contents(fopen("playsample.js", "rb"), javascript);
  echo(
      "<!DOCTYPE html>"
      "<html>"
      "<body><a href='_blank' src='LICENSE.txt'>LICENSE.txt</a>"
      "<div style='display:grid;grid-template-columns: 2fr 3fr'>"
      "   <div class='grid-span-3'>");
  readsf(sf2);
  for (int i = 0; i < 128; i++) {
    PresetZones pz = findByPid(i, 0);
    zone_t *zones = pz.zones;
    shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);

    echo("<details>");
    fprintf(output, "   <summary> %s - %d (%d presets) </summary>", pz.hdr.name,
            pz.hdr.pid, pz.npresets);
    echo(
        "<table border=1 style='border-width:1px'>"
        "<thead><tr>"
        "<td>keyrange</td>"
        "<td>velrange</td>"
        "<td></td>"
        "<td colspan='4'></td><td></td>"
        "</tr></thead><tbody>");

    for (int z = 0; z < pz.npresets; z++) {
      echo("<tr>");
      fprintf(output, "<td>%hu-%hu</td>", zones->VelRange.lo,
              zones->VelRange.hi);
      fprintf(output, "<td> %hu-%hu </td>", zones->KeyRange.lo,
              zones->KeyRange.hi);
      fprintf(output,
              "<td><a href='#' class='pcm' sr='%d' file='%s' "
              "range='bytes=%d-%d' endloop='%d' startloop='%d'>sample"
              " (%d smpls )</a></td>",
              sampl->sampleRate, filename, sdtastart + 2 * sampl->start,
              sdtastart + 2 * sampl->end + 1, sampl->endloop - sampl->start,
              sampl->startloop - sampl->start, sampl->end - sampl->start);

      fprintf(output,
              "<td>%d</td><td>%.1f - %.f</td><td>%d</td><td>%d</td><td>%d</td>",
              zones->Attenuation, centtone2freq(zones->FilterFc),
              centdblut(zones->FilterQ),
              (zones->OverrideRootKey > -1 ? zones->OverrideRootKey
                                           : sampl->originalPitch) *
                      100 +
                  zones->CoarseTune * 100 + zones->FineTune,
              sampl->sampleRate, sampl->endloop - sampl->startloop);

      short *attrs = (short *)zones;
      fprintf(output, "<td><a href='#' class='attlist' attrs='");
      for (int i = 0; i < 60; i++) {
        fprintf(output, "%hd%s", attrs[i], i < 59 ? "," : "");
      }
      fprintf(output, "'>attrs</a></td>");

      echo("</tr>");
      zones++;
    }
    echo("</tbody></table></details>");
  }

  echo(
      "</div>"
      "<div id=details style='position: sticky; right: 10px; "
      "top:20px;'></div>"
      "</div> "
      "<script type='module'>");
  echo(javascript);
  echo(
      "</script>"
      "</body>"
      "</html>");
}