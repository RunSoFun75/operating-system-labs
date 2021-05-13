#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SERVER_SOCKET "socket"

void printUpperText(int fd) {
    char* buf;
    if ((buf = malloc(BUFSIZ * sizeof(char))) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    size_t count;
    while ((count = read(fd, buf, BUFSIZ))) {
        for (size_t i = 0; i < count; ++i) {
                buf[i] = toupper(buf[i]);
        }
        write(STDOUT_FILENO, buf, count);
    }
    free(buf);
}

void cleanup(int sockfd) {
    close(sockfd);
    unlink(SERVER_SOCKET);
}

int main(int argc, char *argv[]) {
    int sockfd;
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr;
    struct sockaddr *cast_addr = (struct sockaddr*)&addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, SERVER_SOCKET, sizeof(addr.sun_path));

    if (bind(sockfd, cast_addr, sizeof(addr)) == -1) {
        perror("bind");
        cleanup(sockfd);
        exit(1);
    }

    if (listen(sockfd, 1) == -1) {
        perror("listen");
        cleanup(sockfd);
        exit(1);
    }

    socklen_t addr_len;
    int fd = accept(sockfd, cast_addr, &addr_len);
    if (fd == -1) {
        perror("accept");
        exit(1);
    }

    printUpperText(fd);
    close(fd);
    cleanup(sockfd);
    return 0;
}
