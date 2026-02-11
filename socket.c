// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 6969
#define MSG_SIZE 20

static int client_fd, server_fd;

void socket_recv(void *buf) {
    size_t total = 0;
    ssize_t n;

    while (total < MSG_SIZE) {
        n = recv(client_fd, (char*)buf + total, MSG_SIZE - total, 0);
        if (n <= 0) {
            printf("socket_recv() failed!\n");
            exit(1);
        }
        
        total += n;
    }
    return;
}

void socket_send(void *buf) {
    size_t total = 0;
    ssize_t n;

    while (total < MSG_SIZE) {
        n = send(client_fd, (char*)buf + total, MSG_SIZE - total, 0);
        if (n <= 0) {
            printf("socket_send() failed!\n");
            exit(1);
        }
        total += n;
    }
    return;
}

void socket_close() {
    close(client_fd);
    close(server_fd);
    printf("closed sockets!\n");
}

int socket_create() {

    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    printf("waiting for socket connection...\n");
    client_fd = accept(server_fd, NULL, NULL);
    return client_fd;
}