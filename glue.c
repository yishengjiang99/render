void fetch_await_buffer(char* url, char* buf, int len, void* opaque,
                        void (*cb)(void* opaque, void* data)) {
  fread(buf, len, 1, fopen(url, "rb"));
  cb(opaque, buf);
}