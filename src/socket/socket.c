#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
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

void panic(const char *fmt, ...) {
    va_list ap;
    char msg[NET_ERR_LEN];

    if (!msg) return;
    va_start(ap, fmt);
    vsnprintf(msg, NET_ERR_LEN, fmt, ap);
    va_end(ap);

    perror(msg);
    exit(1);
}
