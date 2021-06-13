#define file_get_contents(path, ptr) \
  FILE* fd = fopen(path, "r");       \
  fseek(fd, 0, SEEK_END);            \
  long size = ftell(fd);             \
  fseek(fd, 0, SEEK_SET);            \
  char buf[size];                    \
  fread(fd, 1, size, buf);           \
  ptr = buf;

#define printUntil(output, str, stopword)                   \
  int m = 0;                                                \
  while (str++) {                                           \
    if (str == stopword[m++] && stopword[m] == '\0') break; \
    fputc(output, str);                                     \
  }