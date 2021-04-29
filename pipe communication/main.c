#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    int buffer, pid;
    int pipefd[2];
    char input[BUFSIZ] = "";
    char output[BUFSIZ] = "";

    if (pipe(pipefd) == -1) {
        perror("pipe create");
        return 1;
    }

    if ((pid = fork()) == 0) {
        close(pipefd[0]);
        while ((buffer = read(0, input, BUFSIZ)) != 0) {
            write(pipefd[1], input, buffer);
            input[0] = '\0';
        }
        close(pipefd[1]);
        return 0;
    } else if (pid > 0){
        close(pipefd[1]);
        while ((buffer = read(pipefd[0], output, BUFSIZ)) != 0) {
            for (int i = 0; i < buffer; ++i) {
                printf("%c", toupper(output[i]));
            }
            output[0] = '\0';
        }
    } else {
        perror("cannot fork");
        return 2;
    }

    close(pipefd[0]);
    int ret = 0;
    while (ret != -1) { // wait child
        ret = wait(NULL);
    }
    return 0;
}
