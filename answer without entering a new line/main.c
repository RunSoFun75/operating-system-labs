#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    struct termios save;
    struct termios tty;

    int fd = open("/dev/tty", O_RDONLY);
    char ch;

    if (!isatty(fd)) {
        exit(1);
    }

    tcgetattr(fd, &tty);
    save = tty;
    tty.c_cflag &= ~(ICANON|ISIG);
    tty.c_cc[VMIN] = 1;
    tcsetattr(fd, 0, &tty);
    char q[3][100];
    strcpy(q[0], "print n [y/n]");
    strcpy(q[1], "print n [y/n]");
    strcpy(q[2], "print y [y/n]");
    char answer[3] = {'n', 'n', 'y'};
    int i = 0;
    for (int i = 0; i < 3; ++i) {
        puts(q[i]);
        read(fd, &ch, 1);
        if (ch == answer[i]) {
            puts("\nCorrect!");
        } else if (ch == 'n' || ch == 'y') {
            puts("\nNot a correct answer!");
        } else if (ch == 'q') {
            break;
        } else {
            puts("\nNot a correct sym");
        }
    }
    tcsetattr(fd, 0, &save);
    return 0;
}
