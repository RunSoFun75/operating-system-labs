#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int buffer = 0;
    char input[BUFSIZ];
    char output[BUFSIZ];
    FILE *fpin, *fpout;

    if (argc == 1) {
        if ((fpin = popen("./lab26 w", "r")) == NULL) {
            perror("./lab26 w");
            return 1;
        }
        if ((fpout = popen("./lab26 r", "w")) == NULL) {
            perror("./lab26 r");
            return 1;
        }

        while (fgets(input, BUFSIZ, fpin) != NULL) {
            fputs(input, fpout);
            fflush(NULL);
        }

        pclose(fpin);
        pclose(fpout);
    }

    if (argv[1] && strcmp(argv[1], "w") == 0) {
        while ((buffer = read(0, input, BUFSIZ)) != 0) {
            write(fileno(stdout), input, buffer);
            input[0] = '\0';
        }
    }
    if (argv[1] && strcmp(argv[1], "r") == 0) {
        while ((buffer = read(0, output, BUFSIZ)) != 0) {
            for (int i = 0; i < buffer; ++i) {
                printf("%c", toupper(output[i]));
            }
            output[0] = '\0';
        }
    }
    return 0;
}
