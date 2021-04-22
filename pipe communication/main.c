#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    int pid, i, rtn;
    int fd[2];
    static char *lines = {"ThIsIsOrIgInAlTeXt\n"};
    char input[1000];
    if (pipe(fd) == -1) {
        perror(argv[0]);
        exit(1);
    }
    if ((pid = fork()) > 0) {
        close(fd[0]);
        write(fd[1], lines, strlen(lines));
        close(fd[1]);
    } else if (pid == 0) {
        close(fd[1]);
        while ((rtn = read(fd[0], input, 1000)) > 0) {
            for (i = 0; i < rtn; i++) {
                if (islower(input[i])) {
                    input[i] = toupper(input[i]);
                }
            }
            write(1, input, rtn);
        }
        close(fd[0]);
    } else {
        perror(argv[0]);
        exit(2);
    }
    return 0;
}