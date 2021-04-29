#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>

#define MSGSZ 128

struct message_buf {
    long mtype;
    char mtext[MSGSZ];
} message_buf = {1L, "\0"};

int main(int argc, char *argv[]) {
    int msg_fd;
    if ((msg_fd = msgget((int) getuid(), IPC_CREAT | 0640)) == -1) {
        perror("can't msgget");
        exit(1);
    }

    while (fgets(message_buf.mtext, MSGSZ, stdin) != NULL) {
        if (msgsnd(msg_fd, &message_buf, MSGSZ, 0) == -1) {
            perror("can't send");
            exit(3);
        }
    }
    return 0;
}
