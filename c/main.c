#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

unsigned int interrupt_count = 0;

void handleInterrupt() {
    interrupt_count++;
    if (interrupt_count != 4) {
        return;
    }
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t activeTTYs;
    switch (activeTTYs = fork()) {
    case -1:
        perror("fork");
        return;
    case 0:
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        if (close(pipe_fds[0]) != 0)
            perror("close");
        if (close(pipe_fds[1]) != 0) {
            perror("close");
        }

        execl("/bin/ps", "ps", "a", "-o", "tty=", NULL);
        exit(EXIT_FAILURE);
    default: {
        if (close(pipe_fds[1]) != 0)
            perror("close");

        char buff[4096];
        int bytes_read;
        while ((bytes_read = read(pipe_fds[0], buff, sizeof(buff))) > 0) {
            write(STDOUT_FILENO, buff, bytes_read);
        }
        if (bytes_read == -1) {
            perror("read");
        }

        if (close(pipe_fds[0]) != 0)
            perror("close");

        int ws;
        if (waitpid(activeTTYs, &ws, 0) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        if (ws != 0) {
            exit(ws);
        }
    }}
}

int main() {
    signal(SIGINT, handleInterrupt);
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        perror("unable to create pipe");
        return EXIT_FAILURE;
    }
    pid_t ps;
    switch (ps = fork()) {
    case -1:
        perror("fork");
        return EXIT_FAILURE;
    case 0:
        printf("[C] enter\n");
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return EXIT_FAILURE;
        }

        if (close(pipe_fds[0]) != 0)
            perror("close");
        if (close(pipe_fds[1]) != 0)
            perror("close");

        execl("/bin/ps", "ps", "a", "-o", "pid=", NULL);
        exit(EXIT_FAILURE);
        break;
    default: {
        printf("[P] enter\n");
        if (close(pipe_fds[1]) != 0)
            perror("close");

        char buff[4096];
        int bytes_read;
        while ((bytes_read = read(pipe_fds[0], buff, sizeof(buff))) > 0) {
            write(STDOUT_FILENO, buff, bytes_read);
        }
        if (bytes_read == -1) {
            perror("read");
        }

        if (close(pipe_fds[0]) != 0)
            perror("close");

        int ws;
        if (waitpid(ps, &ws, 0) == -1) {
            perror("wait");
            return EXIT_FAILURE;
        }
        if (ws != 0) {
            return ws; // exit with exit code of ps
        }
        printf("[P] wait complete\n");
        while(getchar() != 'q');
    }}
    return 0;
}

