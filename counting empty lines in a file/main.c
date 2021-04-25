#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char input[BUFSIZ];
    FILE *fin, *fpout;

    if ((fin = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        return 1;
    }
    if ((fpout = popen("wc -l", "w")) == NULL) {
        perror("popen");
        return 1;
    }

    while (fgets(input, BUFSIZ, fin) != NULL) {
        if (strlen(input) == 1 && input[0] == '\n') {
            fputs(input, fpout);
            fflush(NULL);
        }
    }
    fclose(fin);
    pclose(fpout);
    return 0;
}