#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"

static struct sockaddr_in* create_addr(int socket_fd, char* bindaddr, int port);
int server(char *bindaddr, int port);

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

// start a tcp server that listens the given port and address
int server(char *bindaddr, int port) {
    int socket_fd = create_socket();

    struct sockaddr_in* socket_addr = create_addr(socket_fd, bindaddr, port);
    try_bind(socket_fd, socket_addr);
    try_listen(socket_fd);

    return socket_fd;
}

int main() {
    int server_fd = server(BIND_ADDR, BIND_PORT);

    while (1) {
        struct sockaddr_in socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);

        char* client_hostname = inet_ntoa(socket_addr.sin_addr);
        printf("Client: %s\n", client_hostname);

        char buf[256];
        read(client_fd, buf, sizeof(buf));
        printf("Client message: %s\n", buf);

        char msg[] = "Hello World!";
        write(client_fd, msg, sizeof(msg));
        close(client_fd);
    }
    return 0;
}
