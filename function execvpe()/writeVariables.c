#include <stdio.h>

extern char **environ;

int main(int argc, char* argv[]) {
    char **p;
    printf("argv:\n");
    for (int i = 0; i < argc; ++i) {
        printf("  %2d: '%s'\n", i, argv[i]);
    }
    printf("environ variables:\n");
    for (p = environ; *p != 0; ++p) {
        printf("  %s\n", *p);
    }
    return 0;
}