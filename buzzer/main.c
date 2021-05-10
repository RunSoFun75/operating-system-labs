#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define BEL '\007'

int sounds = 0;
int is_end = 0;

void sound() {
    sounds++;
}

void end() {
    is_end++;
}

int main(int argc, char *argv[]) {
    struct sigaction sigct1, sigct2;
    sigct1.sa_handler = sound;
    sigct2.sa_handler = end;
    sigaction(SIGINT, &sigct1, NULL);
    sigaction(SIGQUIT, &sigct2, NULL);
    while (1) {
        pause();
        printf("%c\n", BEL);
        if (is_end) {
            printf("amount of sounds: %d\n", sounds);
            exit(0);
        }
    }
}