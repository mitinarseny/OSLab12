#include <unistd.h>
#include <fcntl.h>
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

    int pipe1_fds[2];
    if (pipe(pipe1_fds) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
        return;
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    case 0: // child
        if (close(STDIN_FILENO) != 0) {
            perror("close");
        }
        if (dup2(pipe1_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
            return;
        }

        if (close(pipe1_fds[0]) != 0)
            perror("close");
        if (close(pipe1_fds[1]) != 0) {
            perror("close");
        }
        
        execl("/bin/ps", "ps", "a", "-o", "tty=", NULL);
        exit(EXIT_FAILURE);
        return;
    }
    if (close(pipe1_fds[1]) != 0)
        perror("close");

    int pipe2_fds[2];
    if (pipe(pipe2_fds) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
        return;
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    case 0: // child
        if (close(pipe2_fds[0]) != 0)
            perror("close");

        if (dup2(pipe1_fds[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
            return;
        }
        if (close(pipe1_fds[0]) != 0) 
            perror("close");

        if (dup2(pipe2_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
            return;
        }
        if (close(pipe2_fds[1]) != 0)
            perror("close");

        execl("/usr/bin/sort", "sort", NULL);
        exit(EXIT_FAILURE);
        return;
    }

    if (close(pipe2_fds[1]) != 0)
        perror("close");

    pid_t uniq;
    switch (uniq = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    case 0: // child
        if (dup2(pipe2_fds[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        if (close(pipe2_fds[0]) != 0)
            perror("close");
        printf("Active terminals:\n");
        execl("/usr/bin/uniq", "uniq", NULL);
        exit(EXIT_FAILURE);
        return;
    }

    int ws;
    if (waitpid(uniq, &ws, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
        return;
    }

    if (ws != 0) {
        exit(ws); // exit with last process non-zero exit code
        return;
    }
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
            return ws; // exit with non-zero exit code from ps
        }

        while(getchar() != 'q');
    }}
    return 0;
}

