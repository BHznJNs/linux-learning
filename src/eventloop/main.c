#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "eventloop.h"
#include "../socket/socket.h"
#include "../utils.h"

#define IP_STR_LEN 46
#define IO_BUF_LEN 1024

static struct sockaddr_in* create_addr(int socket_fd, char* bindaddr, int port);
static void remove_client(EventLoop* eventloop, int client_fd);
static void read_from_client(EventLoop* eventloop, int client_fd);
static void reply_to_client(EventLoop* eventloop, int client_fd);
static void accept_handler(EventLoop* eventloop, int fd);
static int init_server(void);

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

static void remove_client(EventLoop* eventloop, int client_fd) {
    Event* ev = eventloop->event_head;
    if (ev->fd == client_fd) {
        eventloop->event_head = ev->next;
        free(ev);
        ev = eventloop->event_head;
    }

    while (ev->next) {
        if (ev->next->fd == client_fd) {
            Event* target_ev = ev->next;
            ev->next = target_ev->next;
            free_event(target_ev);
            continue;
        }
        ev = ev->next;
    }
}

static void read_from_client(EventLoop* eventloop, int client_fd) {
    char buf[IO_BUF_LEN];
    int nread = read(client_fd, buf, IO_BUF_LEN);
    if (nread < 0) {
        logger("ERROR", "client read error");
        remove_client(eventloop, client_fd);
    } else if (nread == 0) {
        logger("INFO", "client %d closed", client_fd);
        remove_client(eventloop, client_fd);
        return;
    } else {
        logger("INFO", "client %d: %s", client_fd, buf);
    }
    Event* reply_event = create_event(client_fd, WRITE_EVENT, reply_to_client);
    append_event(eventloop, reply_event);
}

static void reply_to_client(EventLoop* eventloop, int client_fd) {
    char msg[] = "message from server";
    write(client_fd, msg, sizeof(msg));
    logger("INFO", "reply: %s", msg);
    remove_client(eventloop, client_fd);
}

static void accept_handler(EventLoop* eventloop, int server_fd) {
    struct sockaddr_in socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);

    char client_ip[IP_STR_LEN];
    int client_port;
    inet_ntop(AF_INET, (void*)&socket_addr.sin_addr, client_ip, sizeof(client_ip));
    client_port = ntohs(socket_addr.sin_port);
    logger("INFO", "accepted client: %s:%d", client_ip, client_port);

    Event* read_event = create_event(client_fd, READ_EVENT, read_from_client);
    append_event(eventloop, read_event);
}

static int init_server(void) {
    int server_fd = create_socket();
    struct sockaddr_in* socket_addr = create_addr(server_fd, BIND_ADDR, BIND_PORT);
    try_bind(server_fd, socket_addr);
    try_listen(server_fd);
    return server_fd;
}

int main() {
    int server_fd = init_server();

    EventLoop* eventloop = create_eventloop();
    Event* accept_event = create_event(server_fd, READ_EVENT, accept_handler);
    append_event(eventloop, accept_event);

    start_eventloop(eventloop);
    return 0;
}