#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <aio.h>

#define SERVER_SOCKET "socket"

#define DEBUG(S) write(STDERR_FILENO, (S), sizeof(S))
#define LAB32_AIO_SIGNAL SIGUSR1

struct server {
    sigset_t mask_aio;
    int sockfd;
    char *address_path;
    size_t client_count;
    struct aiocb clients[SOMAXCONN];
    size_t buf_size;
} server;

void cleanup(int sockfd, const char *socket_path) {
    close(sockfd);
    unlink(socket_path);
}

void safeCleanup(struct server *this) {
    cleanup(this->sockfd, this->address_path);
    for (int i = 0; i < this->client_count; ++i) {
        if (this->clients[i].aio_fildes == -1) {
            continue;
        }
        close(this->clients[i].aio_fildes);
    }
}

int configureServer(const char *socket_path) {
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
        exit(1);
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("listen");
        cleanup(sockfd, socket_path);
        exit(1);
    }
    return sockfd;
}

int getFirstSuitableIndex(struct server *this) {
    int i = 0;
    while (this->clients[i].aio_fildes != -1) {
        ++i;
    }
    return i;
}

void addClient(struct server *this, int fd) {
    sigprocmask(SIG_BLOCK, &this->mask_aio, NULL);

    const int index = getFirstSuitableIndex(this);

    if (index >= this->client_count) {
        if ((this->clients[index].aio_buf = malloc(this->buf_size)) == NULL) {
            perror("malloc return NULL");
            exit(1);
        }
        this->client_count++;
    }
    this->clients[index].aio_fildes = fd;

    aio_read(&this->clients[index]);

    sigprocmask(SIG_UNBLOCK, &this->mask_aio, NULL);
}

void createServer(struct server *this, const char *socket_path) {
    memset(this, 0, sizeof(*this));

    this->sockfd = configureServer(socket_path);
    if (this->sockfd == -1) {
        exit(1);
    }

    if ((this->address_path = malloc(strlen(socket_path) + 1)) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    strcpy(this->address_path, socket_path);

    this->buf_size = BUFSIZ;
    for (size_t i = 0; i < SOMAXCONN; ++i) {
        this->clients[i].aio_fildes = -1;

        this->clients[i].aio_nbytes = this->buf_size;

        this->clients[i].aio_sigevent.sigev_value.sival_int = i;
        this->clients[i].aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        this->clients[i].aio_sigevent.sigev_signo = LAB32_AIO_SIGNAL;
    }

    sigemptyset(&this->mask_aio);
    sigaddset(&this->mask_aio, LAB32_AIO_SIGNAL);
}

void CleanServer(struct server *this) {
    safeCleanup(this);
    for (size_t i = 0; i < this->client_count; ++i) {
        free((void*) this->clients[i].aio_buf);

        if (this->clients[i].aio_fildes == -1) {
            continue;
        }
        close(this->clients[i].aio_fildes);
    }

    free(this->address_path);
}

void interrupt() {
    write(STDERR_FILENO, "\nInterrupted. Exiting...\n", sizeof("Interrupted. Exiting...\n"));
    safeCleanup(&server);
    _exit(EXIT_SUCCESS);
}

void removeClient(struct server *this, size_t index) {
    if (index >= this->client_count) {
        return;
    }

    if (this->clients[index].aio_fildes == -1) {
        return;
    }

    close(this->clients[index].aio_fildes);
    this->clients[index].aio_fildes = -1;
}

void aio_handler(int signum, siginfo_t *siginfo, void *vunused) {
    for (size_t i = 0; i < server.client_count; ++i) {
        if (server.clients[i].aio_fildes == -1) {
            continue;
        }

        const int err = aio_error(&server.clients[i]);
        if (err == EINPROGRESS || err == ECANCELED) {
            continue;
        }

        if (err > 0) {
            DEBUG("I/O Error\n");
            safeCleanup(&server);
            _exit(EXIT_FAILURE);
        }

        const ssize_t count = aio_return(&server.clients[i]);

        if (count == 0) {
            removeClient(&server, i);
            continue;
        }

        char *buf = (char*)server.clients[i].aio_buf;
        for (size_t i = 0; i < count; ++i) {
                buf[i] = toupper(buf[i]);
        }
        write(STDOUT_FILENO, buf, count);
        aio_read(&server.clients[i]);
    }
}

int main(int argc, char *argv[]) {
    struct sigaction act = {};
    act.sa_handler = interrupt;
    sigaction(SIGINT, &act, NULL);

    createServer(&server, SERVER_SOCKET);

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = aio_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(LAB32_AIO_SIGNAL, &act, NULL);

    while (1) {
        int client_fd = accept(server.sockfd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            CleanServer(&server);
            exit(1);
        }
        addClient(&server, client_fd);
    }
}
