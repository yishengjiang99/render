#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "sf2/sf2.c"

FILE *output, *sf2;
char outputff[1024];
int matched;
#define echo(str) fprintf(output, "%s", str);

#define min(a,b) a<b ? a : b
static inline void file_get_contents(FILE *fd, char *ptr) {
  fseek(fd, 0, SEEK_END);
  long size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  fread(ptr, 1, size, fd);
}

const char *notestr[12] = {"A",  "A#", "B", "C",  "C#", "D",
                           "D#", "E",  "F", "F#", "G",  "G#"};

void printHTML(char* readff);
              
int main(int argc, char **argv) {
  char *readff = "file.sf2";

  readsf(readff);
  output = fopen("index2.html", "w");

  echo(
      "<!DOCTYPE html>"
      "<html><head>"
      "<style>"
      "  body{background-color:black;color:white;margin:0;}"
      "  main{display:grid;grid-template-columns:1fr 1fr;}"
      "aside{    position: fixed;top: 20px;right: 0px;    display: grid;}"
      "</style></head>"
      "<body>"
      "<header>"
      "</header>"
      "<script type='module' src='./playsample.js'></script><main><div>");
  for (int i = 0; i < 128; i++) {
    PresetZones *pz = findByPid(i, 0);
    if (pz->npresets == 0) continue;
    zone_t *zones = pz->zones;
    shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);

    echo("<details>");
    fprintf(output, "   <summary> %s - %d %d (%d presets) </summary>",
            pz->hdr.name, pz->hdr.pid, pz->hdr.bankId, pz->npresets);
    echo(
        "<table border=1 style='border-width:1px'>"
        "<thead><tr>"
        "<td>keyrange</td>"
        "<td>velrange</td>"
        "<td>attenuate</td><td>modfilter</td><td>mod2pitch</td>"
        "<td>mod_sustain</td>"
        "<td>filter</td>"
                "<td colspan=4>lfomods</td>"
        "<td>pitch</td><td>samprate</td><td>loopLength</td>"
        "</tr></thead><tbody>");

    for (int z = 0; z < pz->npresets; z++) {
      echo("\n<tr>");
      float pitch = (zones->OverrideRootKey > -1 ? zones->OverrideRootKey
                                                 : sampl->originalPitch) *
                        100.0f +
                    zones->CoarseTune * 100 + zones->FineTune;

      fprintf(output,
              "<td>%hu-%hu</td>"
              "<td> %hu-%hu </td>"
              "<td>%hd</td>"
              "<td>%hd</td><td>%hd</td><td>%hd</td>"
              "<td>%hu(%hd)</td>"
              "<td>%f</td>"              
              "<td>%hd</td><td>%hd</td><td>%hd</td><td>%hd</td>"
              "<td>%d</td>"
              "<td>%u|%u|%u</td></tr>",
              zones->KeyRange.lo, zones->KeyRange.hi, zones->VelRange.lo,
              zones->VelRange.hi, zones->Attenuation, 
              zones->ModEnv2FilterFc, zones->ModEnv2Pitch, zones->ModEnvSustain, 
              zones->FilterFc,zones->FilterQ, 
              pitch, 
                zones->ModLFO2Pitch,    zones->VibLFO2Pitch,   zones->ModLFO2FilterFc,   zones->ModLFO2Vol,
              sampl->sampleRate,
              sampl->startloop - sampl->start, sampl->endloop - sampl->start,
              sampl->end - sampl->endloop);

      short *attrs = (short *)zones;
            fprintf(output, "<tr><td colspan=12><a class='attlist' sr='%d' file='%s' "
              "range='bytes=%u-%u' endloop='%u' startloop='%u' "
              "pitch='%f' zone='", 
              sampl->sampleRate, readff, sdtastart + 2 * sampl->start,
              sdtastart + 2 * sampl->end + 1, sampl->endloop-sampl->start, sampl->startloop-sampl->start,
              pitch);

      for (int i = 0; i < 60; i++) {
        fprintf(output, "%hd%s", attrs[i], i < 59 ? "," : "");
      }
      fprintf(output, "'>attrs</a>");
      fprintf(output, "<a midi='%d' class='pcm'>%f</a>&nbsp;",
              (int)(pitch / 100), pitch);
      for (int i = zones->KeyRange.lo;
           i <= zones->KeyRange.lo + 34 && i <= zones->KeyRange.hi; i++) {
        if (i < 21 || i > 100) continue;
        fprintf(output,
                "<a href='javascript:;' midi='%d' class='pcm'>%d%s</a>&nbsp;",
                i, i / 12, notestr[i % 12]);
      }

      zones++;
    }
    echo("</tbody></table></details>");
  }

  echo(
      "</div>"
      "</main><footer></footer>"
      "<aside id='details'></aside>"

      "</body>"
      "</html>");
#ifdef debugger
  system("chrome-cli open https://dsp.grepawk.com/render/file.html");
#endif
}