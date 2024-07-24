#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket.h"

#define MAX_CONNECTION 4
#define IP_STR_LEN 46

static struct sockaddr_in* create_addr(int socket_fd, char* bindaddr, int port);
static int nonblock_accept(int server_fd, struct sockaddr* socket_addr, socklen_t* len);
static int server(char *bindaddr, int port);

// --- --- --- --- --- ---

static struct sockaddr_in* create_addr(int socket_fd, char* bindaddr, int port) {
    struct sockaddr_in* socket_addr = malloc(sizeof(struct sockaddr_in));
    socket_addr->sin_family = AF_INET; // IPv4
    socket_addr->sin_port = htons(port); // set port
    socket_addr->sin_addr.s_addr = htonl(INADDR_ANY); // accept any address

    if (bindaddr) {
        // convert a address string, like "127.0.0.1",
        // into number form, like 0x7f000001,
        // and assign it to `socket_addr->sin_addr`
        int status_code = inet_pton(AF_INET, bindaddr, &socket_addr->sin_addr);
        if (status_code == 0) {
            close(socket_fd);
            panic("Invalid bind address\n");
        }
    }
    return socket_addr;
}

static void log_ip_port(struct sockaddr_in* socket_addr, socklen_t socket_addr_len) {
    char client_ip[IP_STR_LEN];
    int client_port;

    inet_ntop(AF_INET, (void*)&socket_addr->sin_addr, client_ip, sizeof(client_ip));
    client_port = ntohs(socket_addr->sin_port);
    printf("[INFO] Accepted: %s:%d\n", client_ip, client_port);
}

static int nonblock_accept(int server_fd, struct sockaddr* socket_addr, socklen_t* len) {
    int client_fd;
    // do {
    //     client_fd = accept(server_fd, socket_addr, len);
    // } while(client_fd == -1 && errno == EINTR);
    // if (client_fd == -1) {
    //     return client_fd;
    // }
    // set_nonblock(client_fd);

    do {
        client_fd = accept4(server_fd, socket_addr, len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    } while(client_fd == -1 && errno == EINTR);

    return client_fd;
}

static int server(char *bindaddr, int port) {
    int socket_fd = create_socket();

    struct sockaddr_in* socket_addr = create_addr(socket_fd, bindaddr, port);
    try_bind(socket_fd, socket_addr);
    try_listen(socket_fd);

    return socket_fd;
}

int main() {
    int server_fd = server(BIND_ADDR, BIND_PORT);
    int client_fds[MAX_CONNECTION];

    while (1) {
        printf("[DEBUG] Loop started\n");
        int client_index = 0;

        for (int i=0; i<MAX_CONNECTION; i++) {
            struct sockaddr_in socket_addr;
            socklen_t socket_addr_len = sizeof(socket_addr);

            int client_fd = nonblock_accept(server_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
            if (client_fd == -1) {
                break;
            }

            client_fds[client_index] = client_fd;
            client_index += 1;
            log_ip_port(&socket_addr, socket_addr_len);
        }

        int max_fd = 0;
        fd_set rfds, wfds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        for (int i=0; i<client_index; i++) {
            int fd = client_fds[i];
            FD_SET(fd, &rfds);
            FD_SET(fd, &wfds);
            if (max_fd < fd) {
                max_fd = fd;
            }
        }

        struct timeval tv;
        tv.tv_sec = 4;
        tv.tv_usec = 0;

        int ret_val;
        ret_val = select(max_fd + 1, &rfds, NULL, NULL, &tv);
        printf("[Select] return value: %d\n", ret_val);
        if (ret_val > 0) {
            for (int i=0; i<client_index; i++) {
                int fd = client_fds[i];
                if (FD_ISSET(fd, &rfds)) {
                    char buf[256];
                    int ret = read(fd, buf, sizeof(buf));
                    printf("[DEBUG] read ret = %d\n", ret);
                    printf("[Client %d] %s\n", fd, buf);
                }
            }
        }

        for (int i=0; i<client_index; i++) {
            close(client_fds[i]);
        }
        printf("[DEBUG] Loop end\n");
    }
    return 0;
}
