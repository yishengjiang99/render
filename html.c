#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "gen_ops.h"
#include "sf2.c"
#define echo(str) fprintf(stdout, str);

void printpreset(int pid, int bkdrums) {
#define getRange(attr) ((rangesType *)attrs[k])
  char misc[1024];
  uint32_t miscoffset = 0;
  PresetZones pz = findByPid(pid, bkdrums ? 0 : 128);
  fprintf(stdout, "<div style='overflow-x:auto;'>");
  fprintf(stdout, "   <table class='table-auto'>");
  echo(
      "<thead><td colspan=3>mod2pitch</td><td>FilterFc</td><td>FilterQ</td><td "
      "colspan=2>mod_lpf</td><td>ModLFO2Vol</td><td>ChorusSend</"
      "td><td>ReverbSend</td><td>Pan</td><td>ModLFODelay</td><td>ModLFOFreq</"
      "td><td>VibLFODelay</td><td>VibLFOFreq</td><td "
      "colspan=6>volume-delay/att/hold/decay/sus/release</"
      "td><td>Key2VolEnvHold</td><td>Key2VolEnvDecay</td><td>Instrument</"
      "td><td>Reserved1</td><td>KeyRange</td><td>VelRange</td><td>Keynum</"
      "td><td>Velocity</td><td>Attenuation</td><td>Reserved2</"
      "td><td>CoarseTune</td><td>FineTune</td><td>SampleId</"
      "td><td>SampleModes</td><td>Reserved3</td><td>ScaleTune</"
      "td><td>ExclusiveClass</td><td>OverrideRootKey</td><td>Dummy</td></"
      "thead>");
  zone_t *zones = pz.zones;
  for (int j = 0; j < pz.npresets; j++) {
    fprintf(stdout, "<tr>");
    short *attrs = (short *)zones;
    for (int k = 0; k < 60; k++) {
      switch (k) {
        case VelRange:
          fprintf(stdout, "<td>%hu-%hu</td>", zones->VelRange.lo,
                  zones->VelRange.hi);
          break;
        case KeyRange:
          fprintf(stdout, "<td>%hu-%hu</td>", zones->KeyRange.lo,
                  zones->KeyRange.hi);

          break;
        case StartAddrOfs:
        case EndAddrOfs:
        case StartLoopAddrOfs:
        case EndLoopAddrOfs:
        case StartAddrCoarseOfs:
        case EndAddrCoarseOfs:
        case StartLoopAddrCoarseOfs:
        case EndLoopAddrCoarseOfs:
        case Unused1:
        case Unused2:
        case Unused3:
        case Unused4:
          continue;
          break;
        case ModEnvAttack:
        case ModEnvDecay:
        case ModEnvRelease:
        case ModEnvSustain:
        case ModEnvHold:

          misc[miscoffset] = attrs[k];
          miscoffset += 4;
          break;
        default:
          fprintf(stdout, "<td>%hd</td>", attrs[k]);
          break;
      }
    }
    fprintf(stdout, "</tr>");
    zones++;
  }
  fprintf(stdout, "  </table>");
  fprintf(stdout, "</div>");
}
#include <signal.h>
#include <unistd.h>
volatile uint32_t UART_sic_msg = 0;
uint8_t bitshift = 0;
void onsig(int signo) {
  switch (signo) {
    case SIGUSR1:
      bitshift++;
      break;
    case SIGUSR2:
      bitshift++;
      break;
    default:
      break;
  }
}
int main() {
  echo(
      "<!DOCTYPE HTML><head><link rel='stylesheet' "
      "href='tailwind.min.css'></head><html>");
  echo("<pre>");
  readsf(fopen("GeneralUserGS.sf2", "rb"));
  echo("</pre>");
  echo("<select id='phdrsel'>");
  for (int i = 0; i < nphdrs; i++) {
    if (phdrs[i].bankId != 0) continue;
    fprintf(stdout, "<option value='%d'>%s</option>", i, phdrs[i].name);
  }
  echo("</select>");

  pid_t pid;
  // setProgram(0, 0);
  fprintf(stdout, "<body><main class='container'>");
  printpreset(0, 0);

  signal(SIGUSR1, onsig);
  signal(SIGUSR2, onsig);
  echo("\n");
}