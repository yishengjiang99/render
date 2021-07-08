

#define printUntil(output, str, stopword)                   \
  int m = 0;                                                \
  while (str++) {                                           \
    if (str == stopword[m++] && stopword[m] == '\0') break; \
    fputc(output, str);                                     \
  }
#define openurl(url) system("chrome-cli open '" url "'")
#define v9(js) system("chrome-cli execute '" js "'")

#define bmpheader(buf, bytes, offset) \
  buf[0] = 0x42;                      \
  buf[1] = 0x4D;                      \
  (buf + 6) = (unsigned int)bytes;    \
  (buf + 10) = (unsigned int)offset;
