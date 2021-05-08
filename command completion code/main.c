#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    int status;
    if (fork() == 0) {
        execvp(argv[1], &argv[1]);
        perror(argv[1]);
        exit(127);
    }
    int ret = 0;
    while (ret != -1) {
        ret = wait(&status);
    }
    printf("exit status: %d\n", WEXITSTATUS(status));
    exit(0);
}