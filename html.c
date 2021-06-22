#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>

#ifndef CMAKEON
#define debug 1
#include "sf2/sf2.c"

#else
#include "sf2/sf2.h"
#endif
FILE *output, *sf2;
char outputff[1024];
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
  char *readff = argc > 2 ? argv[2] : "file.sf2";

  char *outputff = argc > 3 ? argv[3] : "file.html";

  readsf(readff);

  output = fopen(outputff, "w");
  file_get_contents(fopen("playsample.js", "rb"), javascript);
  echo(
      "<!DOCTYPE html>"
      "<html><head></head>"
      "<body><a href='_blank' src='base64w'>LICENSE.txt</a>"
      "<script src='playsample.js'></script> "
      "<div style='display:grid;grid-template-columns: 2fr 3fr'>\n"
      "   <div class='grid-span-3'>");
  for (int i = 0; i < 128; i++) {
    PresetZones pz = findByPid(i, 0);
    if (pz.npresets == 0) continue;
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
        "<td>play</td>"
        "<td>attenuate</td><td>mod_pitch_env</td><td>filter</td>"
        "<td>pitch</td><td>samprate</td><td>loopLength</td>"  
        "</tr></thead><tbody>");

    for (int z = 0; z < pz.npresets; z++) {
      echo("\n<tr>");
      float pitch = (zones->OverrideRootKey > -1 ? zones->OverrideRootKey
                                                 : sampl->originalPitch) *
                        100.0f +
                    zones->CoarseTune * 100 + zones->FineTune;
      fprintf(output, "<td>%hu-%hu</td>", zones->VelRange.lo,
              zones->VelRange.hi);
      fprintf(output, "<td> %hu-%hu </td>", zones->KeyRange.lo,
              zones->KeyRange.hi);
      fprintf(
          output,
          "<td><a href='javascript:;' class='pcm' sr='%d' file='%s' "
          "range='bytes=%d-%d' endloop='%d' startloop='%d' pitch='%.1f'>sample</a></td>",
          sampl->sampleRate, readff, sdtastart + 2 * sampl->start,
          sdtastart + 2 * sampl->end + 1,  sdtastart + sampl->endloop-sampl->start,
           sdtastart + sampl->startloop - sampl->start,  pitch);

            fprintf(output,"<td>%hd</td>",  zones->Attenuation);
             fprintf(output,      "<td>%hd</td>",  zones->ModEnv2FilterFc);
           fprintf(output,      "<td>%hd(%hd)</td>",        zones->FilterFc, zones->FilterQ);
             fprintf(output,      "<td>%f</td>",  pitch);
     fprintf(output,     
             "<td>%u</td>"
             "<td>%u</td>"
             "<td>%u</td>",
           
              sampl->sampleRate, sampl->endloop - sampl->startloop, sampl->end - sampl->start);

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

      "</body>"
      "</html>");
      #ifdef debugger
      system("chrome-cli open https://dsp.grepawk.com/render/file.html");
      #endif
}