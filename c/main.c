#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "errors.h"

int main() {
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        errExit("unable to create pipe");
    }
    if (fork() == 0) {
        close(1);
        dup2(pipe_fds[1], 1);
        execl("/bin/ps", "ps", "ax");
        close(pipe_fds[1]);
    } else {
        int ps_status;
        if (wait(&ps_status) == -1) {
            errExit("unable to wait for the child process to complete");
        }
        close(0);
        dup2(pipe_fds[0], 0);
        char *buf;
        printf("%d bytes was read", read(0, buf, 1));
        printf(buf);
    }
}

