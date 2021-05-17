#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    struct flock lock;
    int fd;
    char command[100];
    if ((fd = open(argv[1], O_RDWR)) == -1) {
        perror(argv[1]);
        exit(1);
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        if ((errno == EACCES) || (errno == EAGAIN)) {
            printf("%s busy -- try later\n", argv[1]);
            exit(2);
        }
        perror(argv[1]);
        exit(2);
    }
    sprintf(command, "nano %s\n", argv[1]);
    system(command);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 0;
}
