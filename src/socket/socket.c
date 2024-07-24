#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "socket.h"

int create_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        panic("socket: %s\n", strerror(errno));
    }

    // set socket to allow OS reuse address
    int status_code, opt = 1;
    status_code = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (status_code == -1) {
        close(socket_fd);
        panic("setsockopt SO_REUSEADDR: %s\n", strerror(errno));
    }

    return socket_fd;
}

void set_nonblock(int socket_fd) {
    int old_flags = fcntl(socket_fd, F_GETFL);
    int new_flags = old_flags | O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, new_flags) == -1) {
        close(socket_fd);
        panic("fcntl(F_SETFL,O_NONBLOCK): %s\n", strerror(errno));
    }
}

void set_nodelay(int socket_fd) {
    int yes = 1;
    int status_code = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    if (status_code == -1) {
        panic("setsockopt TCP_NODELAY: %s\n", strerror(errno));
    }
}

void try_connect(int socket_fd, struct sockaddr_in* socket_addr, size_t addr_len) {
    int status_code = connect(socket_fd, (struct sockaddr*)socket_addr, sizeof(struct sockaddr));
    if (status_code == -1 && errno != EINPROGRESS) {
        close(socket_fd);
        panic("connect: %s\n", strerror(errno));
    }
}

void try_bind(int socket_fd, struct sockaddr_in* socket_addr) {
    int status_code = bind(socket_fd, (struct sockaddr*)socket_addr, sizeof(struct sockaddr));
    if (status_code == -1) {
        close(socket_fd);
        panic("socket bind: %s\n", strerror(errno));
    }
}

void try_listen(int socket_fd) {
    int status_code = listen(socket_fd, 64);
    if (status_code == -1) {
        close(socket_fd);
        panic("listen: %s\n", strerror(errno));
    }
}

void check_connect(int socket_fd) {
    int socket_err;
    socklen_t len = sizeof(socket_err);
    int status_code = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &socket_err, &len);
    if (status_code == -1) {
        close(socket_fd);
        panic("getsockopt");
    }

    if (socket_err != 0) {
        fprintf(stderr, "Connection failed with error: %d\n", socket_err);
        close(socket_fd);
    } else {
        printf("Connection successful.\n");
    }
}

void panic(const char *fmt, ...) {
    va_list ap;
    char msg[256];

    if (!msg) return;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    perror(msg);
    exit(1);
}
