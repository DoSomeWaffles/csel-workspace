#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define FIRST_PROCESS "Hello, I'm the first process"
#define RECEIVED "OK, I received your message"
#define FIRST_MESSAGE "FIRST_MESSAGE"
#define SECOND_MESSAGE "SECOND_MESSAGE"
#define THIRD_MESSAGE "THIRD_MESSAGE"
#define FOURTH_MESSAGE "FOURTH_MESSAGE"
#define EXIT_MESSAGE "exit"

void catch_signal(int signo)
{
    /* do something... */
    /* to terminate process execution with
     * 1) success: exit(EXIT_SUCCESS);
     * 2) failure: exit(EXIT_FAILURE);
     */
    printf("Signal receive\n");
    printf(signo)
}

void send_message(int fd, char* message)
{
    write(fd, message, sizeof(message));
}

int main()
{
    int fd[2];
    char buf[30];

    struct sigaction act = {
        .sa_handler = catch_signal,
    };
    sigaction(SIGHUP, &act, NULL);

    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if (err == -1) printf("Error socketpair");

    cpu_set_t set;
    CPU_ZERO(&set);  // clear the set
    pid_t pid = fork();
    if (pid == 0)  // Child
    {
        CPU_SET(1, &set);  // add cpu
        sched_setaffinity(0, sizeof(set), &set);
        close(fd[1]);  // close the parent one
        send_message(fd[0], FIRST_MESSAGE);
        read(fd[0], &buf, sizeof(buf));
        send_message(fd[0], SECOND_MESSAGE);
        read(fd[0], &buf, sizeof(buf));
        send_message(fd[0], THIRD_MESSAGE);
        read(fd[0], &buf, sizeof(buf));
        sleep(100);
        send_message(fd[0], EXIT_MESSAGE);
        read(fd[0], &buf, sizeof(buf));
        close(fd[0]);

    } else if (pid > 0)  // Parent
    {
        CPU_SET(0, &set);
        sched_setaffinity(0, sizeof(set), &set);
        close(fd[0]);  // close the child one
        while (1) {
            read(fd[1], &buf, sizeof(buf));
            if (strcmp(buf, EXIT_MESSAGE) == 0) {
                write(fd[1], RECEIVED, sizeof(RECEIVED));
                printf("exit\n");
                close(fd[1]);
                return 0;
            }
            printf("%s\n", buf);
            write(fd[1], RECEIVED, sizeof(RECEIVED));
        }
    } else
        printf("Error create child");

    return 0;
}