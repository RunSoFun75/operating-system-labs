#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_SOCKET "socket"

void writeFile(int fd, const char* filename) {
    const int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("open");
        return;
    }

    char* buf;
    if ((buf = malloc(BUFSIZ * sizeof(char))) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    size_t count;
    while ((count = read(file, buf, BUFSIZ))) {
        write(fd, buf, count);
    }
    free(buf);
    close(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        exit(1);
    }

    const char *filename = argv[1];

    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr;
    struct sockaddr* cast_addr = (struct sockaddr*) &addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, SERVER_SOCKET, sizeof(addr.sun_path));

    if (connect(sockfd, cast_addr, sizeof(addr))) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    writeFile(sockfd, filename);
    close(sockfd);
    return 0;
}
