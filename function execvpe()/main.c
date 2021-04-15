#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int execvpe(const char *file, char *argv[], char *envp[]) {
    for (int i = 0; envp[i] != NULL; ++i) {
        if(putenv(envp[i]) == -1) {
            perror("putenv");
        }
    }
    if (execvp(file, argv) == -1) {
        perror("execvp");
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char *args[] = {"AB", "BC", "CD", "DE", "EF", NULL};
    char *envp[] = {"A=B", "B=C", "C=D", "D=E"};
    execvpe("./file", args, envp);
    return 0;
}