#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();
    if (pid == 0) {
        int a = 0;
        while (1) {
            a++;
            if (a == -1) {
                printf("%d", a);
            }
        }
        exit(0);
    } else if (pid > 0) {
        int a = 0;
        while (1) {
            a++;
            if (a == -1) {
                printf("%d", a);
            }
        }
        // wait child to finishes
        int status = 0;
        waitpid(pid, &status, 0);
    } else {
        printf("fork failed\n");
        exit(1);
    }
}