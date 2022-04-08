#include <sys/mman.h>
#include <fcntl.h>
#include<stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include<stdint.h>
//0x01c1'4200 à 0x01c1'420c
int main(int argc, char* argv[])
{
    size_t psz = getpagesize();
    off_t phys_addr = 0x01c14200;
    off_t offset = phys_addr % psz;//result is 0x00000200
    off_t page_start = phys_addr -offset;//result is 0x0x01c14000
    uint32_t chip_id[4] = {0,0,0,0};
    int fd = open("/dev/mem", O_RDWR);
    if(fd<0)return -1;
    //start must be page aligned
    //we are mapping 1 page in virtual mem
    volatile uint32_t* ptr = 
        mmap(0,psz,PROT_READ|PROT_WRITE, MAP_SHARED,fd,page_start);//gives us the ptr which points at the start of the virtual page
    if(ptr==MAP_FAILED){
        printf("MAP FAILED");
        return -1;
    }
    
    int i;
    for(i = 0;i<4;i++){
        chip_id[i] = *(ptr + i  +  offset/ sizeof(uint32_t));
    }

    printf("chipid = %08x' %08x' %08x' %08x'", chip_id[0],chip_id[1],chip_id[2],chip_id[3]);
    munmap((void*)ptr,psz);
    close(fd);
    return 0;
}