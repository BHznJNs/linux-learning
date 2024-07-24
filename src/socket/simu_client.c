#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include "socket.h"

#define CLIENT_COUNT 4

static struct sockaddr_in* create_addr(char *addr, int port);
static int nonblock_client(char *addr, int port);

// --- --- --- --- --- ---

static struct sockaddr_in* create_addr(char *addr, int port) {
    struct sockaddr_in* socket_addr = malloc(sizeof(struct sockaddr_in));
    memset(socket_addr, 0, sizeof(socket_addr));
    socket_addr->sin_family = AF_INET; // IPv4
    socket_addr->sin_port = htons(port); // set port

    int status_code = inet_aton(addr, &socket_addr->sin_addr);
    if (status_code != 0) {
        return socket_addr;
    }

    // addr is invalid, maybe a hostname like `localhost`
    struct hostent* he;
    he = gethostbyname(addr);

    if (he == NULL) {
        panic("can't resolve: %s\n", addr);
    }
    memcpy(&socket_addr->sin_addr, he->h_addr, sizeof(struct in_addr));
    return socket_addr;
}

static int nonblock_client(char *addr, int port) {
    int socket_fd = create_socket();
    struct sockaddr_in* socket_addr = create_addr(addr, port);
    set_nonblock(socket_fd);
    try_connect(socket_fd, socket_addr, sizeof(socket_addr));
    return socket_fd;
}

int main(int argc, char** argv) {
    int client_fds[CLIENT_COUNT];

    for (int i=0; i<CLIENT_COUNT; i++) {
        client_fds[i] = nonblock_client(BIND_ADDR, BIND_PORT);
    }

    int max_fd = 0;
    fd_set rfds, wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    for (int i=0; i<CLIENT_COUNT; i++) {
        int fd = client_fds[i];
        FD_SET(fd, &rfds);
        FD_SET(fd, &wfds);
        if (fd > max_fd) {
            max_fd = fd;
        }
    }

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int ret_val;
    ret_val = select(max_fd + 1, NULL, &wfds, NULL, &tv);
    printf("select return value: %d\n", ret_val);
    if (ret_val > 0) {
        short index = 1;
        for (int i=0; i<CLIENT_COUNT; i++) {
            int fd = client_fds[i];
            check_connect(fd);
            if (FD_ISSET(fd, &wfds)) {
                char msg[20];
                sprintf(msg, "Test content %d", index);
                printf("[DEBUG] write content: %s\n", msg);
                write(fd, msg, strlen(msg));

                index += 1;
            }
        }
    }

    for (int i=0; i<CLIENT_COUNT; i++) {
        close(client_fds[i]);
    }
    return 0;
}