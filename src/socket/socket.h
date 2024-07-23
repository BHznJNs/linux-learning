#ifndef __NET_SOCKET_H__
#define __NET_SOCKET_H__

#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define BIND_ADDR "127.0.0.1"
#define BIND_PORT 8000

int create_socket();
void set_nodelay(int socket_fd);
void set_nonblock(int socket_fd);

void try_connect(int socket_fd, struct sockaddr_in* socket_addr, size_t addr_len);
void try_bind(int socket_fd, struct sockaddr_in* socket_addr);
void try_listen(int socket_fd);

void check_connect(int socket_fd);

void panic(const char *fmt, ...);

#endif
