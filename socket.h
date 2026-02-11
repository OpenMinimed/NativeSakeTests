#ifndef SOCKET
#define SOCKET


void socket_recv(void *buf);
void socket_send(void *buf);
void socket_close();
int socket_create();

#endif /* SOCKET */
