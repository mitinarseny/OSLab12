#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include "errors.h"

int main() {
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0)
        errExit("unable to create pipe");
    printf("pipe_fds: %d, %d\n", pipe_fds[0], pipe_fds[1]);
    pid_t childPID;
    switch (childPID = fork()) {
    case -1:
        errExit("fork() failed");
        break;
    case 0:
        /* close(0); */
        printf("[C] enter\n");
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
            errExit("dup2");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        execl("/bin/ps", "ps", "ax", NULL);
        exit(EXIT_FAILURE);
        break;
    default:
        printf("[P] enter\n");
        close(pipe_fds[1]);
        char buff[4096];
        int bytes_read;
        while ((bytes_read = read(pipe_fds[0], buff, sizeof(buff))) > 0) {
            write(STDOUT_FILENO, buff, bytes_read);
        }
        if (bytes_read == -1)
            errExit("read");
        close(pipe_fds[0]);
        int ws;
        if (wait(&ws) == -1)
            errExit("wait");
        if (ws != 0)
            errExit("child process terminated with status code not zero");
        printf("[P] wait complete\n");
        
        break;
    }
    return 0;
    /* if (fork() == 0) { */
    /*     printf("[C] enter\n"); */
    /*     close(1); */
    /*     printf("[C] closed 1\n"); */
    /*     close(pipe_fds[0]); */
    /*     printf("[C] closed pipe_fds[0]\n"); */
    /*     dup2(pipe_fds[1], 1); */
    /*     printf("[C] dup2 pipe_fds[1] -> 1\n"); */
    /*     close(pipe_fds[1]); */
    /*     printf("[C] closed pipe_fds[1]\n"); */
    /*     execl("/bin/ps", "ps", "ax", NULL); */
    /* } else { */
    /*     printf("[P] enter\n"); */
    /*     int ps_status; */
    /*     if (wait(&ps_status) == -1) { */
    /*         errExit("unable to wait for the child process to complete"); */
    /*     } */
    /*     printf("[P] wait complete\n"); */
    /*     close(0); */
    /*     dup2(pipe_fds[0], 0); */
    /*     char *buf; */
    /*     printf("%d bytes was read", read(0, buf, 1)); */
    /*     printf(buf); */
    /* } */
}

