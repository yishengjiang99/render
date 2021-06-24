#include ""

#include <stdio.h>
      #include <sys/stat.h>
       #include <fcntl.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>

int main(int argc, char** argv) {
    struct stat sb;
    int fd;
   off_t offset;
   size_t length;
   ssize_t s;
      fd=open(argv[1],O_RDONLY);
      


//oid* ff = mmap(NULL, fstat(f //.size, PROT_READ, MAP_PRIVATE, fd, 0) :
}