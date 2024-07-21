#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // char err[NET_ERR_LEN];
    // int client_fd = client(err, BIND_ADDR, BIND_PORT);

    // char msg[] = "Test content\n";
    // write(client_fd, msg, sizeof(msg));

    // close(client_fd);
    return 0;
}