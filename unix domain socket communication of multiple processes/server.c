#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>

#define SERVER_SOCKET "socket"

struct server {
    char *address_path;
    struct pollfd clients[SOMAXCONN];
    size_t client_count;
    char *buf;
    size_t buf_size;
    size_t clients_to_remove_count;
    size_t first_client_to_remove;
};

void cleanup(int sockfd, const char *path) {
    close(sockfd);
    unlink(path);
}

int getSockfd(struct server *this) {
    return this->clients[0].fd;
}

int ConfigureServer(const char *socket_path) {
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un addr;
    struct sockaddr *addrp = (struct sockaddr*)&addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path));

    if (bind(sockfd, addrp, sizeof(addr)) == -1){
        perror("bind");
        cleanup(sockfd, socket_path);
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("listen");
        cleanup(sockfd, socket_path);
        return -1;
    }
    return sockfd;
}

void createServer(struct server *this, const char *socket_path) {
    memset(this, 0, sizeof(*this));

    const int sockfd = ConfigureServer(socket_path);
    if (sockfd == -1) {
        exit(1);
    }

    if ((this->address_path = malloc(strlen(socket_path) + 1)) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    strcpy(this->address_path, socket_path);

    for (size_t i = 0; i < SOMAXCONN; ++i) {
        this->clients[i].events = POLLIN;
    }

    this->clients[this->client_count++].fd = sockfd;

    this->buf_size = BUFSIZ;
    if ((this->buf = malloc(this->buf_size)) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
}

void cleanServer(struct server *this) {
    cleanup(getSockfd(this), this->address_path);
    free(this->buf);
    free(this->address_path);
}

void removeClient(struct server *this, size_t index) {
    if (index == 0)  {
        return;
    }

    if (this->clients[index].fd == -1) {
        return;
    }

    close(this->clients[index].fd);
    this->clients[index].fd = -1;
    this->clients_to_remove_count++;

    if (this->first_client_to_remove == 0 || this->first_client_to_remove > index) {
        this->first_client_to_remove = index;
    }
}

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void removeClients(struct server *this) {
    if (!this->clients_to_remove_count) {
        return;
    }
    if (!this->first_client_to_remove) {
        return;
    }

    size_t removed_count = 0;

    size_t j = this->client_count - 1;

    for (size_t i = this->first_client_to_remove;
         removed_count < this->clients_to_remove_count && i < j; ++i) {

        if (this->clients[i].fd == -1) {
            while (this->clients[j].fd == -1 && j > i) {
                j--;
                removed_count++;
            }
            swap(&this->clients[i].fd, &this->clients[j].fd);
            j--;
            removed_count++;
        }
    }

    this->client_count -= this->clients_to_remove_count;

    this->clients_to_remove_count = 0;
    this->first_client_to_remove = 0;
}

void tryRead(struct server *this, size_t readable_count) {
    size_t read_fd = 0;
    for (size_t i = 1; read_fd <= readable_count && i < this->client_count; ++i) {
        if (this->clients[i].revents & POLLIN) {
            read_fd++;

            const ssize_t count = read(this->clients[i].fd, this->buf, this->buf_size);
            for (size_t j = 0; j < count; ++j) {
                    this->buf[j] = toupper(this->buf[j]);
            }
            write(STDOUT_FILENO, this->buf, count);

            if (count == -1) {
                perror("read");
                removeClient(this, i);
            }

            if (count == 0) {
                removeClient(this, i);
            }
        }
    }
    removeClients(this);
}

int main(int argc, char *argv[]) {
    struct server server;
    createServer(&server, SERVER_SOCKET);

    size_t fd_count;
    while ((fd_count = poll(server.clients, server.client_count, -1)) != -1) {
        int has_pending = server.clients[0].revents & POLLIN;
        if (has_pending) {
            const int client_fd = accept(getSockfd(&server), NULL, NULL);
            if (client_fd == -1) {
                perror("accept");
                cleanServer(&server);
                exit(1);
            }
            server.clients[server.client_count++].fd = client_fd;
        }
        tryRead(&server, has_pending ? fd_count - 1 : fd_count);
    }
    perror("poll");
    cleanServer(&server);
    return 0;
}
