#define TML_IMPLEMENTATION
#include "tml.h"
tml_message *m;

tml_message *next(int msec) {

  if (m->next->time >= msec) {
    m = m->next;
    return m;
  } else {
    return 0;
  }
}
int main(int argc, char **argv) {
  m = tml_load_filename(argc > 2 ? argv[2] : "song.mid");
  next(4);
  return 1;
}