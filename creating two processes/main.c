#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    pid_t pid, ret;
    sigset_t newset;
    sigfillset (&newset);
    sigprocmask(SIG_BLOCK, &newset, 0);
    int status = 0;
    if (argc < 2) {
        printf("pass file as argument, please!\n");
        return 0;
    }
    if ((pid = fork()) == 0) {
        sleep(20);
        execl("/bin/cat", "cat", argv[1], (char *) 0);
    }
    else {
        printf("parent: waiting for child: %ld\n", pid);
    }

    int ret = 0;
    while (ret != -1) {
        ret = wait(&status); // wait child
    }

    printf("parent: wait's return value: %ld,", ret);
    printf("child's status: %d\n", WEXITSTATUS(status));
    exit(0);
}