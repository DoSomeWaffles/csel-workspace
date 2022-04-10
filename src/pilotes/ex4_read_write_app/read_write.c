#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char* text ="text to write test\n";

int main(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("Not enough arguments\n");
        return 0;
    }
    // writing to the device
    int fdw = open(argv[1], O_RDWR);
    printf("%d\n", fdw);
    write(fdw, argv[1], strlen(argv[1]));
    write(fdw, argv[2], strlen(argv[2]));
    write(fdw, text, strlen(text));
    close(fdw);

    // reading the device
    int fdr = open(argv[1], O_RDONLY);
    while (1) {
        char buff[10000]; // with a bigger buffer, it seems to go faster
       
        ssize_t sz = read(fdr, buff, sizeof(buff) - 1);
        printf("%zd\n", sz);
        if (sz <= 0) break;
        buff[sizeof(buff) - 1] = 0;
        printf("%s", buff);
    }
    close(fdr);

    return 0;
}