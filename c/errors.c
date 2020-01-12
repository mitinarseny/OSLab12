#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "errors.h"

const size_t BUF_SIZE = 100;

void errExit(const char *msg) {
//    char buf[BUF_SIZE];
//    strcat(buf, msg);
//    printf(buf);
//    strcat(buf, ": ERROR%d: %s\n")
//    sprintf(buf, BUF_SIZE, "ERROR%d: %s\n", errno, strerror(errno));
    errMsg(msg);
    _exit(errno);
}

void errMsg(const char *msg) {
    fprintf(stderr, "%s: ERROR%d: %s\n", msg, errno, strerror(errno));
    fflush(stderr);
}