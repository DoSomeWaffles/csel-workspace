#include <sys/mman.h>
#include <fcntl.h>
#include<stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <stdio.h>

//0x01c1'4200 à 0x01c1'420c
#define START_ADDR 0x01c1'4200
#define END_ADDR 0x01c1'4200
int main(int argc, char* argv[])
{
    UNUSED(argc); UNUSED(argv);
    size_t psz = getpagesize();
    int phys_addr = 0x01c1'4200;
    int offset = phys_addr % psz;
    int page_start = phys_addr -psz;

    int fd = open("/dev/mem", O_RDWR);
    if(fd<0)return -1;
    //start must be page aligned
    int* ptr = mmap(NULL,psz,PROT_READ|PROT_WRITE, MAP_PRIVATE,fd,page_start);//gives us the ptr which points at the start of the virtual page


}