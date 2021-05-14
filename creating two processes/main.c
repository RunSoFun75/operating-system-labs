#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    if (argc < 2) {
        printf("pass file as argument, please!\n");
        return 0;
    }
    if ((pid = fork()) == 0) {
        sleep(20);
        execl("/bin/cat", "cat", argv[1], 0);
    }
    else {
        printf("parent: waiting for child: %ld\n", pid);
    }

    int ret = 0, status = 0;
    while (ret != -1) {
        ret = wait(&status); // wait child
    }
    printf("child's status: %d\n", WEXITSTATUS(status));
    exit(0);
}