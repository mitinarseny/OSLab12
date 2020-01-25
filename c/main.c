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
    pid_t pids[3];
    switch (pids[0] = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    case 0: // child
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
        perror("execl failed");
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

    switch (pids[1] = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    case 0: // child
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
        perror("execl failed");
        exit(EXIT_FAILURE);
        return;
    }
    
    if (close(pipe1_fds[0]) != 0)
        perror("close");

    if (close(pipe2_fds[1]) != 0)
        perror("close");

    switch (pids[2] = fork()) {
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
        perror("execl failed");
        exit(EXIT_FAILURE);
        return;
    }

    for (int i = 0; i < 3; i ++) {
        int ws;
        if (waitpid(pids[i], &ws, 0) == -1)
            perror("waitpid");
    }
}

int main() {
    signal(SIGINT, handleInterrupt);

    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        perror("unable to create pipe");
        return EXIT_FAILURE;
    }

    pid_t psPid;
    switch (psPid = fork()) {
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

        execl("/bin/ps", "ps", "ax", "-o", "pid=", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    if (close(pipe_fds[1]) != 0)
        perror("close");
     
    printf("Active processes:\n");
    char buff[4096];
    int bytes_read;
    while ((bytes_read = read(pipe_fds[0], buff, sizeof(buff))) > 0) {
        if (write(STDOUT_FILENO, buff, bytes_read) == -1) {
            perror("write");
            return EXIT_FAILURE;
        }
    }
    if (bytes_read == -1) {
        perror("read");
    }

    if (close(pipe_fds[0]) != 0)
        perror("close");

    int ws;
    if (waitpid(psPid, &ws, 0) == -1) {
        perror("wait");
        return EXIT_FAILURE;
    }
    if (ws != 0) {
        printf("ps exited with code %d", ws);
        return ws; // exit with non-zero exit code from ps
    }

    printf("Enter anything to exit: ");
    fflush(stdout);
    getchar();
    return 0;
}

