#include "httpd.h"
#include "httpd.c"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sf2.c"
char *indexhtml;
int main(int c, char **v)
{

    char *addr;
    int fd;
    struct stat sb;
    off_t offset, pa_offset;
    size_t length;
    ssize_t s;
    readsf(fopen("file.sf2", "r"));

    serve_forever("12913");
    return 0;
}

void route()
{
    ROUTE_START()

    ROUTE_GET("/")
    {
        printf("HTTP/1.1 200 OK\r\n");
        printf("Content-Type: text/html \r\n\r\n");
        printf("\r\n");
        FILE *fd = fopen("index.html", "rb");
        char delaybuf[255];
        uint8_t delayPtr = 0x00;
        while (fputc(fgetc(fd), stdout))
        {
        }
        fclose(stdout);
        fclose(fd);
    }
    ROUTE_GET("/js")
    {
    }

    ROUTE_POST("/")
    {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
        printf("Fetch the data using `payload` variable.");
    }

    ROUTE_END()
}
