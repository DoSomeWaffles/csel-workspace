#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NBR_BLOCKS 50
#define MB_SIZE 1024 * 1024

int main()
{
    char* ptr = malloc(NBR_BLOCKS * MB_SIZE);
    if (ptr == NULL) {
        printf("Failed allocation\n");
        return EXIT_FAILURE;
    }
    memset(ptr, 0, NBR_BLOCKS * MB_SIZE);
    printf("All blocks are Zeros\n");

    free(ptr);
    return 0;
}