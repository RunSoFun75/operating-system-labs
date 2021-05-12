/*
 * This lab needs to run two compiled files server, client,
 * you need to run server and then run client from two processes,
 * processes are stopped by a signal CTRL + C (SIGINT)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <ctype.h>

#define MSGSZ 128

struct message_buf {
    long mtype;
    char mtext[MSGSZ];
} message_buf;

int main(int argc, char *argv[]) {
    int msg_fd;
    if ((msg_fd = msgget((int) getuid(), IPC_CREAT | 0640)) == -1) {
        perror("can't msgget");
        exit(1);
    }
    while (1) {
        if (msgrcv(msg_fd, &message_buf, MSGSZ, 1L, 0) == -1) {
            perror("can't recv");
            exit(3);
        }
        for (int i = 0; message_buf.mtext[i] != '\0'; ++ i) {
            printf("%c", toupper(message_buf.mtext[i]));
        }
    }
}