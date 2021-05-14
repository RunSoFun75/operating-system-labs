#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int bufferOut;
    FILE *fp[2];

    if (p2open("sort -n", fp) == -1) {
        perror("sort -n");
        return 1;
    }

    for (int i = 0; i < 100; ++i) {
        fprintf(fp[0], "%d\n", rand() % 100);
    }
    fclose(fp[0]);

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            fscanf(fp[1], "%d", &bufferOut);
            printf("%d ", bufferOut);
        }
        printf("\n");
    }
    fclose(fp[1]);
    return 0;
}
