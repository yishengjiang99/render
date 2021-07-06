#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "sf2/sf2.c"

FILE *output, *sf2;
char outputff[1024];
int matched;
#define echo(str) fprintf(output, "%s", str);

#define min(a, b) a < b ? a : b
static inline void file_get_contents(FILE *fd, char *ptr) {
  fseek(fd, 0, SEEK_END);
  long size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  fread(ptr, 1, size, fd);
}

const char *notestr[12] = {"A",  "A#", "B", "C",  "C#", "D",
                           "D#", "E",  "F", "F#", "G",  "G#"};

void printHTML(char *readff);
const char *ext = ".xml";
int main(int argc, char **argv) {
  char *readff = argc > 1 ? argv[1] : "file.sf2";
  readsf(readff);
  output = fopen(mktemp("u"),"w");

  echo("<?xml version=\"1.0\" encoding=\"ascii\"?>");
  echo("<List>");
  for (int i = 0; i < 128; i++) {
    PresetZones *pz = findByPid(i, 0);
    if (!pz || pz->npresets == 0) continue;
    zone_t *zones = pz->zones;
    shdrcast *sampl = (shdrcast *)(shdrs + zones->SampleId);

    fprintf(output,
            "<Preset pid='%d' bankid='%d'> "
            "<Name><![CDATA[%s]]></Name>"
            "<size>%d</size>",
            pz->hdr.pid, pz->hdr.bankId, pz->hdr.name, pz->npresets);

    for (int z = 0; z < pz->npresets; z++) {
      float pitch = (zones->OverrideRootKey > -1 ? zones->OverrideRootKey
                                                 : sampl->originalPitch) *
                        100.0f +
                    zones->CoarseTune * 100 + zones->FineTune;

      short *attrs = (short *)zones;

      fprintf(output,
              "<Zone lokey='%d' hikey='%d' lovel='%d' hivel='%d' "
              "class='attlist' sr='%d' file='%s' "
              "range='bytes=%u-%u' endloop='%u' startloop='%u' "
              "pitch='%.0f' zone='", zones->KeyRange.lo,zones->KeyRange.hi,zones->VelRange.lo,zones->KeyRange.hi,
              sampl->sampleRate, readff, sdtastart + 2 * sampl->start,
              sdtastart + 2 * sampl->end + 1, sampl->endloop - sampl->start,
              sampl->startloop - sampl->start, pitch);

      for (int i = 0; i < 60; i++) {
        fprintf(output, "%hd%s", attrs[i], i < 59 ? "," : "");
      }
      fprintf(output, "'>sample</Zone>\n");

      zones++;
    }
    echo("</Preset>")
  }
  echo("</List>");
}
//   echo(
//       "</div>"
//       "</main><footer></footer>"
//       "<aside id='details'></aside>"

//       "</body>"
//       "</html>");
// #ifdef debugger
//   system("chrome-cli open https://dsp.grepawk.com/render/file.html");
// #endif
// }