#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>

#define BUFSIZE 257

void createArrayOffsets(int fd, long *fileOffsets, int *lineLength) {
    int i = 1, j = 0;
    char c;
    fileOffsets[1] = 0L;
    while (read(fd, &c, 1)) {
        if (c == '\n') {
            j++;
            lineLength[i++] = j;
            fileOffsets[i] = lseek(fd, 0L, 1);
            j = 0;
        } else {
            j++;
        }
    }
}

int main(int argc, char *argv[]) {
    long fileOffsets[101];
    int fd1, fd2, lineNumber, bufSize = 257, lineLength[101] = {-1};
    char *buf, symbol;
    if ((buf = (char *) malloc(257 * sizeof(char))) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    if ((fd1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1) {
        perror("/dev/tty");
        exit(2);
    }
    if ((fd2 = open(argv[1], O_RDONLY)) == -1) {
        perror(strcat("Input file - ", argv[1]));
        exit(2);
    }

    createArrayOffsets(fd2, fileOffsets, lineLength);

    struct pollfd fds;
    fds.fd = fd1;
    fds.events = POLLIN;
    printf("five seconds to write a line number\n");
    while (1) {
        int ret = poll(&fds, 1, 5000);
        if (ret == -1) {
            perror("incorrect input");
            exit(1);
        } else if (ret == 0) {
            int i = 0;
            lseek(fd2, 0, SEEK_SET);
            while ((i = read(fd2, buf, BUFSIZE)) > 0) {
                write(1, buf, i);
            }
            exit(0);
        } else {
            symbol = read(fd1, buf, BUFSIZE);
            buf[symbol] = '\0';
            lineNumber = atoi(buf);
            if (lineNumber == 0) {
                exit(0);
            } else if (lineNumber < 0 || lineNumber > 100 || lineLength[lineNumber] == -1) {
                fprintf(stderr, "wrong line number\n");
            }
            lseek(fd2, fileOffsets[lineNumber], SEEK_SET);
            if (lineLength[lineNumber] > bufSize) {
                if (realloc(buf, lineLength[lineNumber] * sizeof(char)) == NULL) {
                    perror("realloc returns NULL");
                    exit(1);
                }
                bufSize = lineLength[lineNumber];
            }
            if (read(fd2, buf, lineLength[lineNumber]) > 0) {
                write(1, buf, lineLength[lineNumber]);
            } else {
                fprintf(stderr, "wrong line number\n");
            }
        }
    }
}