#define file_get_contents(path) \
  FILE *fd = fopen(path "r");   \
  fseek(fd, 0, SEEK_END);       \
  long size = ftell(fd);        \
  fseek(fd, 0, SEEK_SET);       \
  char buf[size];               \
  fread(fd, 1, size, buf);\
