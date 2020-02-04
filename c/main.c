#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

pid_t runPS2Stdin(const char[]);
int printPIDs();
int printTTYs();
void handleInterrupt();

int main() {
    struct sigaction act;
    act.sa_flags = SA_RESTART;
    act.sa_handler = &handleInterrupt;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    /* signal(SIGINT, handleInterrupt); */

    pid_t activeProcessesPID;
    switch (activeProcessesPID = fork()) {
    case -1:
        perror("fork");
        return EXIT_FAILURE;
    case 0: {
        int ws;
        if ((ws = printPIDs()) != 0) {
            return ws;
        }
    }}
    
    getchar();
    return 0;
}

unsigned int interruptCount = 0;

// handleInterrupt is a signal handler
void handleInterrupt(int signo) {

    /* struct sigaction act; */
    /* act.sa_handler = &handleInterrupt; */
    /* if (sigaction(SIGINT, &act, NULL) == -1) { */
    /*     perror("sigaction"); */
    /*     exit(EXIT_FAILURE); */
    /* } */
    interruptCount++;
    if (interruptCount != 4)
        return;

    pid_t activeTTYsPID;
    switch (activeTTYsPID = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        exit(printTTYs());
    }

    int ws;
    if (waitpid(activeTTYsPID, &ws, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    exit(ws);

}

// runPS2Stdin executes ps in separate process
// and redirects output to stdin
// return value:
//   -1: error
//   >0: PID
pid_t runPS2Stdin(const char flags[]) {
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        perror("unable to create pipe");
        return -1;
    }

    pid_t psPid;
    switch (psPid = fork()) {
    case -1:
        perror("fork");
        return -1;
    case 0:
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return -1;
        }

        if (close(pipe_fds[0]) != 0)
            perror("close");
        if (close(pipe_fds[1]) != 0)
            perror("close");

        execl("/bin/ps", "ps", flags, NULL);
        perror("execl failed");
        return -1;
    }

    if (close(pipe_fds[1]) != 0)
        perror("close");
   
    if (dup2(pipe_fds[0], STDIN_FILENO) == -1) { // scanf reads from stdin
        perror("dup2");
        return -1;
    }

    if (close(pipe_fds[0]) != 0)
        perror("close");

    return psPid;  
}

// printPIDs prints PIDs of all active processes
// return value:
//   0: on success
//   EXIT_FAILURE: internal error
//   >0: ps non-zero exit code
int printPIDs() {
    pid_t psPid;
    if ((psPid = runPS2Stdin("ax")) == -1) {
        return -1; 
    }

    printf("Active processes:\n");
   
    scanf("%*[^\n]\n"); // skip first line
    int gotPID;
    while (scanf("%d %*[^\n]\n", &gotPID) == 1) {
        printf("%d\n", gotPID);
    }

    int ws;
    if (waitpid(psPid, &ws, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    return ws;
}

// printTTYs prints PIDs of all active processes
// return value:
//   0: on success
//   EXIT_FAILURE: internal error
//   >0: ps non-zero exit code
int printTTYs() {
    pid_t psPid;
    if ((psPid = runPS2Stdin("a")) == -1)
        return -1;

    printf("Active terminals:\n");

    scanf("%*[^\n]\n"); // skip first line

    size_t cap = 1 * sizeof(char*);
    char **ttys = (char **)(malloc(cap));
    int size = 0;

    char *curTTY;
    while(scanf("%*d %ms %*[^\n]\n", &curTTY) == 1) {
        /* printf("%s\n", tty); */
        /* free(ttys[size]); */
        int8_t found = 0;
        for (int i = 0; i < size; i++) {
            if (strcmp(ttys[i], curTTY) == 0) {
                found = 1;
                break;
            }
        }
        if (found == 1) {
            free(curTTY);
            continue;
        }
        printf("%s\n", curTTY);
        ttys[size] = curTTY;
        size++;
        if (size == cap) {
            cap *= 2;
            ttys = realloc(ttys, cap);
        }
    }

    int ws;
    if (waitpid(psPid, &ws, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    return ws;
}
